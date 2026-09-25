#pragma once

// Demo-local application code, NOT a new library API or production recovery service.
#include <array>
#include <iostream>
#include <memory>
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/rand.h>
#include <string>
#include <utility>
#include <vector>

#include <cbmpc/api/pve_batch_ac.h>

namespace pve_demo {
using namespace coinbase;
namespace pve = coinbase::api::pve;

// Optional convenience helper. Applications may instead use their approved
// authenticated recipient-encryption SDK. Never reuse proof randomness for delivery.
// Inputs and output must not alias. Publish ciphertext only on success.
inline error_t seal_for_recipient(mem_t ek, mem_t context, mem_t plain, buf_t& out) {
  out.free();
  buf_t rho(32), sealed;
  if (RAND_bytes(rho.data(), rho.size()) != 1) return E_GENERAL;
  const error_t rv = pve::base_pke_default().encrypt(ek, context, plain, rho, sealed);
  if (rv) return rv;
  out = std::move(sealed);
  return SUCCESS;
}

inline error_t open_at_recipient(mem_t dk, mem_t context, mem_t ciphertext, buf_t& out) {
  out.free();
  buf_t plain;
  const error_t rv = pve::base_pke_default().decrypt(dk, context, ciphertext, plain);
  if (rv) return rv;
  out = std::move(plain);
  return SUCCESS;
}

inline void append_field(buf_t& out, mem_t field) {
  cb_assert(field.size >= 0);
  const uint32_t n = static_cast<uint32_t>(field.size);
  const uint8_t length[] = {uint8_t(n >> 24), uint8_t(n >> 16), uint8_t(n >> 8), uint8_t(n)};
  out += mem_t(length, sizeof(length));
  out += field;
}

// Application-owned per-attempt context, not the spec 6.3.3 full-vector wire format.
// Both endpoints build this from the same approved request, not relay-supplied claims.
inline buf_t delivery_context(mem_t backup, mem_t label, mem_t request, mem_t holder, int attempt) {
  cb_assert(attempt >= 0);
  buf_t out(mem_t("cbmpc-demo/pve-ac-per-attempt/secp256k1"));
  for (const mem_t field : {backup, label, request, holder}) append_field(out, field);
  const uint32_t n = static_cast<uint32_t>(attempt);
  const uint8_t index[] = {uint8_t(n >> 24), uint8_t(n >> 16), uint8_t(n >> 8), uint8_t(n)};
  out += mem_t(index, sizeof(index));
  return out;
}

// Only a holder's private key is passed here. The raw partial never leaves this
// function; buf_t cleanses it at destruction. Authorize the recipient key BEFORE calling.
inline error_t make_holder_message(const pve::base_pke_i& backup_pke, const api::access_structure_t& ac,
                                   const pve::leaf_keys_t& pks, mem_t backup, const std::vector<mem_t>& trusted_Qs,
                                   mem_t label, mem_t request, const std::string& holder, int attempt, mem_t holder_dk,
                                   mem_t recipient_ek, buf_t& message) {
  message.free();
  const auto curve = api::curve_id::secp256k1;
  error_t rv = pve::verify_ac(backup_pke, curve, ac, pks, backup, trusted_Qs, label);
  if (rv) return rv;
  buf_t partial;
  rv = pve::partial_decrypt_ac_attempt(backup_pke, curve, ac, backup, attempt, holder, holder_dk, label, partial);
  if (rv) return rv;
  const buf_t context = delivery_context(backup, label, request, mem_t(holder), attempt);
  return seal_for_recipient(recipient_ek, context, partial, message);
}

// The conceptual coordinator has only ciphertexts; no private keys or partials.
inline std::array<buf_t, 2> relay_messages(const std::array<buf_t, 2>& messages) { return messages; }

// Only the recipient opens messages and assembles plaintext quorum shares.
// Holder identities must be authenticated by the application, not inferred from encryption.
inline error_t recover_at_recipient(const pve::base_pke_i& backup_pke, const api::access_structure_t& ac,
                                    const pve::leaf_keys_t& pks, mem_t backup, const std::vector<mem_t>& trusted_Qs,
                                    mem_t label, mem_t request, const std::array<std::string, 2>& holders, int attempt,
                                    mem_t recipient_dk, const std::array<buf_t, 2>& messages, std::vector<buf_t>& out) {
  out.clear();
  const auto curve = api::curve_id::secp256k1;
  error_t rv = pve::verify_ac(backup_pke, curve, ac, pks, backup, trusted_Qs, label);
  if (rv) return rv;
  std::array<buf_t, 2> partials;
  pve::leaf_shares_t quorum;
  for (size_t i = 0; i < holders.size(); ++i) {
    if (!pks.count(holders[i])) return E_BADARG;
    const buf_t context = delivery_context(backup, label, request, mem_t(holders[i]), attempt);
    rv = open_at_recipient(recipient_dk, context, messages[i], partials[i]);
    if (rv) return rv;
    if (!quorum.emplace(holders[i], mem_t(partials[i])).second) return E_BADARG;
  }
  std::vector<buf_t> recovered;
  rv = pve::combine_ac(backup_pke, curve, ac, backup, attempt, label, quorum, recovered);
  if (rv) return rv;
  out = std::move(recovered);
  return SUCCESS;
}

// Fixture setup only: derive expected public values from locally owned synthetic
// scalars, NOT from an incoming backup. Production obtains these from trusted state.
inline std::vector<buf_t> fixture_public_values(const std::vector<mem_t>& xs) {
  std::unique_ptr<EC_GROUP, decltype(&EC_GROUP_free)> group(EC_GROUP_new_by_curve_name(NID_secp256k1), EC_GROUP_free);
  cb_assert(group);
  std::unique_ptr<EC_POINT, decltype(&EC_POINT_free)> point(EC_POINT_new(group.get()), EC_POINT_free);
  cb_assert(point);
  std::vector<buf_t> values;
  for (const auto x : xs) {
    std::unique_ptr<BIGNUM, decltype(&BN_clear_free)> scalar(BN_bin2bn(x.data, x.size, nullptr), BN_clear_free);
    cb_assert(scalar);
    cb_assert(EC_POINT_mul(group.get(), point.get(), scalar.get(), nullptr, nullptr, nullptr) == 1);
    buf_t q(33);
    cb_assert(EC_POINT_point2oct(group.get(), point.get(), POINT_CONVERSION_COMPRESSED, q.data(), q.size(), nullptr) ==
              33);
    values.push_back(std::move(q));
  }
  return values;
}

inline void demonstrate_protected_recovery(const pve::base_pke_i& backup_pke, const api::access_structure_t& ac,
                                           const pve::leaf_keys_t& pks, mem_t backup, mem_t label,
                                           const std::vector<mem_t>& xs, const std::array<std::string, 2>& holders,
                                           const std::array<mem_t, 2>& holder_dks) {
  const auto public_values = fixture_public_values(xs);
  std::vector<mem_t> trusted_Qs;
  for (const auto& q : public_values) trusted_Qs.emplace_back(q);
  // Honest fixture: attempt 0 succeeds. No unbounded retry loop; see README.
  constexpr int attempt = 0;
  for (bool rsa_recipient : {false, true}) {
    const auto keygen = rsa_recipient ? pve::generate_base_pke_rsa_keypair : pve::generate_base_pke_ecies_p256_keypair;
    buf_t recipient_ek, recipient_dk, wrong_ek, wrong_dk, request(32);
    cb_assert(keygen(recipient_ek, recipient_dk) == SUCCESS);
    cb_assert(keygen(wrong_ek, wrong_dk) == SUCCESS);
    cb_assert(RAND_bytes(request.data(), request.size()) == 1);
    // These in-process fixtures stand in for an approved request/recipient and
    // authenticated holder identities. Randomness alone does NOT authorize a request.
    std::array<buf_t, 2> messages;
    for (size_t i = 0; i < holders.size(); ++i) {
      cb_assert(make_holder_message(backup_pke, ac, pks, backup, trusted_Qs, label, request, holders[i], attempt,
                                    holder_dks[i], recipient_ek, messages[i]) == SUCCESS);
    }
    buf_t another;
    cb_assert(make_holder_message(backup_pke, ac, pks, backup, trusted_Qs, label, request, holders[0], attempt,
                                  holder_dks[0], recipient_ek, another) == SUCCESS);
    cb_assert(another != messages[0]);  // Fresh randomness on every encryption.
    const auto delivered = relay_messages(messages);
    const buf_t context = delivery_context(backup, label, request, mem_t(holders[0]), attempt);
    buf_t rejected(mem_t("sentinel"));
    cb_assert(seal_for_recipient(mem_t(), context, mem_t("example partial"), rejected) != SUCCESS);
    cb_assert(rejected.empty());
    rejected = mem_t("sentinel");
    cb_assert(open_at_recipient(wrong_dk, context, delivered[0], rejected) != SUCCESS);
    cb_assert(rejected.size() == 0);
    buf_t changed_backup(backup);
    changed_backup[changed_backup.size() - 1] ^= 1;
    for (const auto& wrong_context :
         {delivery_context(backup, label, mem_t("another-request"), mem_t(holders[0]), attempt),
          delivery_context(backup, label, request, mem_t(holders[1]), attempt),
          delivery_context(backup, label, request, mem_t(holders[0]), attempt + 1),
          delivery_context(backup, mem_t("another-label"), request, mem_t(holders[0]), attempt),
          delivery_context(changed_backup, label, request, mem_t(holders[0]), attempt)}) {
      rejected = mem_t("sentinel");
      cb_assert(open_at_recipient(recipient_dk, wrong_context, delivered[0], rejected) != SUCCESS);
      cb_assert(rejected.size() == 0);
    }
    std::vector<buf_t> recovered = {buf_t(mem_t("sentinel"))};
    const std::array<std::string, 2> duplicate_holders = {holders[0], holders[0]};
    const std::array<buf_t, 2> duplicate_messages = {delivered[0], delivered[0]};
    cb_assert(recover_at_recipient(backup_pke, ac, pks, backup, trusted_Qs, label, request, duplicate_holders, attempt,
                                   recipient_dk, duplicate_messages, recovered) != SUCCESS);
    cb_assert(recovered.empty());
    const std::array<std::string, 2> unknown_holders = {holders[0], "unknown-holder"};
    recovered = {buf_t(mem_t("sentinel"))};
    cb_assert(recover_at_recipient(backup_pke, ac, pks, backup, trusted_Qs, label, request, unknown_holders, attempt,
                                   recipient_dk, delivered, recovered) != SUCCESS);
    cb_assert(recovered.empty());
    auto wrong_Qs = trusted_Qs;
    cb_assert(wrong_Qs.size() > 1);
    wrong_Qs[0] = trusted_Qs[1];
    rejected = mem_t("sentinel");
    cb_assert(make_holder_message(backup_pke, ac, pks, backup, wrong_Qs, label, request, holders[0], attempt,
                                  holder_dks[0], recipient_ek, rejected) != SUCCESS);
    cb_assert(rejected.size() == 0);
    recovered = {buf_t(mem_t("sentinel"))};
    cb_assert(recover_at_recipient(backup_pke, ac, pks, backup, wrong_Qs, label, request, holders, attempt,
                                   recipient_dk, delivered, recovered) != SUCCESS);
    cb_assert(recovered.empty());
    // Fail on each message in turn, including after a valid partial was opened.
    for (size_t i = 0; i < delivered.size(); ++i) {
      auto tampered = delivered;
      tampered[i][tampered[i].size() - 1] ^= 1;
      recovered = {buf_t(mem_t("sentinel"))};
      cb_assert(recover_at_recipient(backup_pke, ac, pks, backup, trusted_Qs, label, request, holders, attempt,
                                     recipient_dk, tampered, recovered) != SUCCESS);
      cb_assert(recovered.empty());
    }
    recovered = {buf_t(mem_t("sentinel"))};
    cb_assert(recover_at_recipient(backup_pke, ac, pks, backup, trusted_Qs, label, request, holders, attempt, wrong_dk,
                                   delivered, recovered) != SUCCESS);
    cb_assert(recovered.empty());
    cb_assert(recover_at_recipient(backup_pke, ac, pks, backup, trusted_Qs, label, request, holders, attempt,
                                   recipient_dk, delivered, recovered) == SUCCESS);
    cb_assert(recovered.size() == xs.size());
    for (size_t i = 0; i < xs.size(); ++i) cb_assert(recovered[i] == buf_t(xs[i]));
    std::cout << "Recipient-protected recovery / " << (rsa_recipient ? "RSA" : "ECIES")
              << ": recovery and negative checks PASS\n";
  }
}
}  // namespace pve_demo
