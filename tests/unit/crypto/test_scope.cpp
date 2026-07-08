#include <gtest/gtest.h>

#include <cbmpc/internal/crypto/base.h>
#include <cbmpc/internal/crypto/base_pki.h>
#include <cbmpc/internal/crypto/scope.h>

#include "utils/test_macros.h"

namespace coinbase::crypto::scope_test {

struct tracked_resource_t {
  int value;
};

int free_count = 0;
int copy_count = 0;

void reset_counters() {
  free_count = 0;
  copy_count = 0;
}

}  // namespace coinbase::crypto::scope_test

namespace coinbase::crypto {

template <>
void scoped_ptr_t<scope_test::tracked_resource_t>::free(scope_test::tracked_resource_t* ptr) {
  scope_test::free_count++;
  delete ptr;
}

template <>
scope_test::tracked_resource_t* scoped_ptr_t<scope_test::tracked_resource_t>::copy(
    scope_test::tracked_resource_t* ptr) {
  scope_test::copy_count++;
  return new scope_test::tracked_resource_t(*ptr);
}

}  // namespace coinbase::crypto

namespace {

using namespace coinbase::crypto;
using coinbase::buf_t;
using coinbase::crypto::scope_test::copy_count;
using coinbase::crypto::scope_test::free_count;
using coinbase::crypto::scope_test::reset_counters;
using coinbase::crypto::scope_test::tracked_resource_t;

TEST(CryptoScope, RsaScopedPtrCopyMovePreservesDecrypt) {
  rsa_prv_key_t original;
  original.generate(RSA_KEY_LENGTH);
  const rsa_pub_key_t pub = original.pub();

  const buf_t label = buf_t("scope-label");
  const buf_t plaintext = buf_t("scope-plaintext");
  drbg_aes_ctr_t drbg(gen_random(32));

  rsa_pke_t::ct_t ciphertext;
  EXPECT_OK(ciphertext.encrypt(pub, label, plaintext, &drbg));

  rsa_prv_key_t moved = std::move(original);
  EXPECT_FALSE(original);
  EXPECT_TRUE(moved);

  buf_t decrypted_move;
  EXPECT_OK(ciphertext.decrypt(moved, label, decrypted_move));
  EXPECT_EQ(decrypted_move, plaintext);

  rsa_prv_key_t copied = moved;
  EXPECT_TRUE(copied);
  EXPECT_TRUE(moved);

  buf_t decrypted_copy;
  EXPECT_OK(ciphertext.decrypt(copied, label, decrypted_copy));
  EXPECT_EQ(decrypted_copy, plaintext);
}

TEST(CryptoScope, ScopedPtrAttachDetachAndNullFree) {
  reset_counters();

  scoped_ptr_t<tracked_resource_t> ptr;
  EXPECT_FALSE(ptr);
  EXPECT_TRUE(!ptr);
  EXPECT_FALSE(ptr.valid());
  EXPECT_EQ(nullptr, ptr.pointer());

  ptr.free();
  EXPECT_EQ(0, free_count);

  auto* raw = new tracked_resource_t{7};
  ptr.attach(raw);
  EXPECT_TRUE(ptr);
  EXPECT_TRUE(ptr.valid());
  EXPECT_EQ(raw, ptr.pointer());
  EXPECT_EQ(7, ptr->value);

  tracked_resource_t* detached = ptr.detach();
  EXPECT_EQ(raw, detached);
  EXPECT_FALSE(ptr);
  EXPECT_EQ(0, free_count);

  delete detached;
}

TEST(CryptoScope, ScopedPtrCopyHandlesNullOwnedAndSelfAssignment) {
  reset_counters();

  scoped_ptr_t<tracked_resource_t> empty;
  scoped_ptr_t<tracked_resource_t> empty_copy(empty);
  EXPECT_FALSE(empty_copy);
  EXPECT_EQ(0, copy_count);

  scoped_ptr_t<tracked_resource_t> owned(new tracked_resource_t{11});
  scoped_ptr_t<tracked_resource_t> copied(owned);
  EXPECT_TRUE(copied);
  EXPECT_NE(owned.pointer(), copied.pointer());
  EXPECT_EQ(11, copied->value);
  EXPECT_EQ(1, copy_count);

  scoped_ptr_t<tracked_resource_t> assigned;
  assigned = owned;
  EXPECT_TRUE(assigned);
  EXPECT_NE(owned.pointer(), assigned.pointer());
  EXPECT_EQ(11, assigned->value);
  EXPECT_EQ(2, copy_count);

  scoped_ptr_t<tracked_resource_t> cleared(new tracked_resource_t{3});
  cleared = empty;
  EXPECT_FALSE(cleared);
  EXPECT_EQ(1, free_count);
  EXPECT_EQ(2, copy_count);

  tracked_resource_t* before_self_assign = owned.pointer();
  owned = owned;
  EXPECT_EQ(before_self_assign, owned.pointer());
  EXPECT_EQ(1, free_count);
  EXPECT_EQ(2, copy_count);
}

TEST(CryptoScope, ScopedPtrMoveHandlesOwnershipTransferAndSelfAssignment) {
  reset_counters();

  scoped_ptr_t<tracked_resource_t> src(new tracked_resource_t{5});
  scoped_ptr_t<tracked_resource_t> dst(new tracked_resource_t{9});
  tracked_resource_t* transferred = src.pointer();

  dst = std::move(src);
  EXPECT_FALSE(src);
  EXPECT_EQ(transferred, dst.pointer());
  EXPECT_EQ(1, free_count);

  dst = std::move(dst);
  EXPECT_EQ(transferred, dst.pointer());
  EXPECT_EQ(1, free_count);

  scoped_ptr_t<tracked_resource_t> moved(std::move(dst));
  EXPECT_FALSE(dst);
  EXPECT_EQ(transferred, moved.pointer());
}

}  // namespace
