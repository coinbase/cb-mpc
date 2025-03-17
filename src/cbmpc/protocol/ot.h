#pragma once
#include <cbmpc/crypto/base.h>
#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/zk/zk_pedersen.h>

namespace coinbase::mpc {

/**
 * @specs:
 * - oblivious-transfer-spec | PVW-BaseOT-2P
 */
struct base_ot_protocol_pvw_ctx_t {
  enum { l = 128 };

  base_ot_protocol_pvw_ctx_t(ecurve_t curve = crypto::curve_p256) : curve(curve) {}

  // Sender input:
  std::vector<buf_t> x0, x1;

  // Receiver input:
  coinbase::bits_t b;

  // Common:
  int m;
  buf_t sid;
  const ecurve_t curve;

  // Receiver private state:
  std::vector<bn_t> r;

  // Message 1: R => S
  std::vector<ecc_point_t> A, B;

  // Message 2: S => R
  std::vector<ecc_point_t> U0, U1;
  std::vector<buf_t> V0, V1;

  auto msg1() { return std::tie(A, B); }
  auto msg2() { return std::tie(U0, V0, U1, V1); }

  using msg1_t = std::tuple<std::vector<ecc_point_t>&, std::vector<ecc_point_t>&>;
  using msg2_t =
      std::tuple<std::vector<ecc_point_t>&, std::vector<buf_t>&, std::vector<ecc_point_t>&, std::vector<buf_t>&>;

  error_t step1_R2S(const coinbase::bits_t& b);
  error_t step2_S2R(const std::vector<buf_t>& x0, const std::vector<buf_t>& x1);
  error_t output_R(std::vector<buf_t>& x);
};

class h_matrix_256rows_t {
 public:
  void alloc(int cols) { buf.alloc(coinbase::bits_to_bytes(cols) * 256); }

  int cols() const { return bytes_to_bits(row_size_in_bytes()); }
  int rows() const { return 256; }
  void set_row(int index, mem_t value) {
    cb_assert(value.size == row_size_in_bytes());
    memmove(get_row(index).data, value.data, value.size);
  }
  mem_t get_row(int index) const { return mem_t(buf.data() + row_size_in_bytes() * index, row_size_in_bytes()); }

  void convert(coinbase::converter_t& converter) { converter.convert(buf); }
  mem_t bin() const { return buf; }

 private:
  buf_t buf;

  int row_size_in_bytes() const { return buf.size() / 256; }
};

template <class T>
T& update_state(T& state, const h_matrix_256rows_t& matrix) {
  return update_state(state, matrix.bin());
}

class v_matrix_256cols_t {
 public:
  void alloc(int rows) { buf.resize(rows); }
  ~v_matrix_256cols_t() { coinbase::secure_bzero(byte_ptr(buf.data()), rows() * 32); }

  int rows() const { return int(buf.size()); }
  int cols() const { return 256; }

  buf256_t& operator[](int index) { return buf[index]; }
  const buf256_t& operator[](int index) const { return buf[index]; }

 private:
  std::vector<buf256_t> buf;
};

// This implements different variations of the OTExtension protocol depending on which functions are called.
//  - OT-Extension-2P
//  - Sender-One-Input-Random-OT-Extension-2P
//  - Sender-Random-OT-Extension-2P
struct ot_ext_protocol_ctx_t {
  // These parameters are hard-wired because they affect each other and changing any single one will require changing
  // the others.
  static const int u = 256;
  static const int d = 3;
  static const int kappa = 128;

  // Sender input:
  std::vector<buf_t> x0, x1;

  // Receiver input:
  coinbase::bits_t b;

  // Common:
  int l;
  buf_t sid;

  // Receiver private:
  v_matrix_256cols_t T;
  coinbase::bits_t r;

  // Message 1: R => S
  h_matrix_256rows_t U;
  std::vector<buf128_t> v0, v1;

  // Message 2: S => R
  std::vector<buf_t> w0, w1;

  auto msg1() { return std::tie(U, v0, v1); }
  auto msg2() { return std::tie(w0, w1); }
  auto msg2_delta() { return std::tie(w1); }

  using msg1_t = std::tuple<h_matrix_256rows_t&, std::vector<buf128_t>&, std::vector<buf128_t>&>;
  using msg2_t = std::tuple<std::vector<buf_t>&, std::vector<buf_t>&>;
  using msg2_delta_t = std::tuple<std::vector<buf_t>&>;

  /**
   * @specs:
   * - oblivious-transfer-spec | OTExtension-1-RtoS-1P
   */
  error_t step1_R2S(mem_t sid, const std::vector<buf_t>& sigma0, const std::vector<buf_t>& sigma1,
                    const coinbase::bits_t& r, int l);
  /**
   * @specs:
   * - oblivious-transfer-spec | OTExtension-2-StoR-1P
   * @notes:
   * - Calling this function means that we are running OT-Extension-2P
   */
  error_t step2_S2R(mem_t sid, const coinbase::bits_t& s, const std::vector<buf_t>& sigma, const std::vector<buf_t>& x0,
                    const std::vector<buf_t>& x1);
  /**
   * @specs:
   * - oblivious-transfer-spec | OTExtension-2-StoR-1P
   * @notes:
   * - Calling this function means that we are running Sender-One-Input-Random-OT-Extension-2P
   */
  error_t step2_S2R_sender_one_input_random(mem_t sid, const coinbase::bits_t& s, const std::vector<buf_t>& sigma,
                                            const std::vector<bn_t>& delta, const mod_t& q, std::vector<bn_t>& x0,
                                            std::vector<bn_t>& x1);

  /**
   * @notes:
   * - This is the function that the above two functions call and contains the actual logic.
   */
  error_t step2_S2R_helper(mem_t sid, const coinbase::bits_t& s, const std::vector<buf_t>& sigma,
                           const bool sender_one_input_random_mode, const std::vector<buf_t>& x0,
                           const std::vector<buf_t>& x1, const std::vector<bn_t>& delta, const mod_t& q,
                           std::vector<bn_t>& x0_out, std::vector<bn_t>& x1_out);

  /**
   * @specs:
   * - oblivious-transfer-spec | OTExtension-Output-R-1P
   */
  error_t output_R(int m, std::vector<buf_t>& x);

  /**
   * @specs:
   * - oblivious-transfer-spec | OTExtension-1-RtoS-1P
   * @notes:
   * - This is the first round of the Sender-Random-OT-Extension-2P protocol.
   *   At the end of it, the receiver gets its output as well.
   */
  error_t sender_random_step1_R2S(mem_t sid, const std::vector<buf_t>& sigma0, const std::vector<buf_t>& sigma1,
                                  const coinbase::bits_t& r, int l, std::vector<buf_t>& x);

  /**
   * @specs:
   * - oblivious-transfer-spec | OTExtension-Output-R-1P
   * @notes:
   * - This is the output phase of the Sender-Random-OT-Extension-2P protocol run by the sender.
   */
  error_t sender_random_output_S(mem_t sid, const coinbase::bits_t& s, const std::vector<buf_t>& sigma, int m, int l,
                                 std::vector<buf_t>& x0, std::vector<buf_t>& x1);
};

// This implements different variations of the full OT protocol (base and extension) depending on which functions are
// called.
// - Full-OT-2P
// - Sender-One-Input-Random-OT-2P
struct ot_protocol_pvw_ctx_t {
  static const int u = ot_ext_protocol_ctx_t::u;
  base_ot_protocol_pvw_ctx_t base;
  ot_ext_protocol_ctx_t ext;

  ot_protocol_pvw_ctx_t(ecurve_t curve = crypto::curve_p256) : base(curve) {}

  auto msg1() { return base.msg1(); }
  auto msg2() { return std::tuple_cat(base.msg2(), ext.msg1()); }
  auto msg3() { return ext.msg2(); }
  auto msg3_delta() { return ext.msg2_delta(); }

  using msg1_t = base_ot_protocol_pvw_ctx_t::msg1_t;
  using msg2_t = std::tuple<std::vector<ecc_point_t>&, std::vector<buf_t>&, std::vector<ecc_point_t>&,
                            std::vector<buf_t>&, h_matrix_256rows_t&, std::vector<buf128_t>&, std::vector<buf128_t>&>;
  using msg3_t = ot_ext_protocol_ctx_t::msg2_t;
  using msg3_delta_t = ot_ext_protocol_ctx_t::msg2_delta_t;

  /**
   * @specs:
   * - oblivious-transfer-spec | Full-OT-2P
   * - oblivious-transfer-spec | Sender-One-Input-Random-OT-2P
   */
  error_t step1_S2R();

  /**
   * @specs:
   * - oblivious-transfer-spec | Full-OT-2P
   * - oblivious-transfer-spec | Sender-One-Input-Random-OT-2P
   */
  error_t step2_R2S(const coinbase::bits_t& r, int l);

  /**
   * @specs:
   * - oblivious-transfer-spec | Full-OT-2P
   */
  error_t step3_S2R(const std::vector<buf_t>& x0, const std::vector<buf_t>& x1);

  /**
   * @specs:
   * - oblivious-transfer-spec | Full-OT-2P
   * @notes:
   * - Exactly as above, but with bn_t instead of buf_t
   */
  error_t step3_S2R(const std::vector<bn_t>& x0, const std::vector<bn_t>& x1, int l);

  /**
   * @specs:
   * - oblivious-transfer-spec | Sender-One-Input-Random-OT-2P
   */
  error_t step3_S2R(const std::vector<bn_t>& delta, const mod_t& q, std::vector<bn_t>& x0, std::vector<bn_t>& x1);

  /**
   * @specs:
   * - oblivious-transfer-spec | Full-OT-2P
   * - oblivious-transfer-spec | Sender-One-Input-Random-OT-2P
   */
  error_t output_R(int m, std::vector<buf_t>& x);

  /**
   * @specs:
   * - oblivious-transfer-spec | Full-OT-2P
   * - oblivious-transfer-spec | Sender-One-Input-Random-OT-2P
   */
  error_t output_R(int m, std::vector<bn_t>& x);
};

}  // namespace coinbase::mpc

namespace coinbase::crypto {
template <>
inline int get_bin_size(const mpc::h_matrix_256rows_t& matrix) {
  return matrix.bin().size;
}
}  // namespace coinbase::crypto