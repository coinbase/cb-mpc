#include "data_generator.h"

namespace coinbase::test {

bn_config_t curve_random_scalar_config(ecurve_t c) {
  if (c == coinbase::crypto::curve_secp256k1) {
    return bn_config_t(DIST(bn)::INT256_POS_0, {bn_filter_t::SECP256K1_COEF_FIELD_0});
  } else if (c == coinbase::crypto::curve_ed25519) {
    return bn_config_t(DIST(bn)::INT256_POS_0, {bn_filter_t::ED25519_COEF_FIELD_0});
  } else if (c == coinbase::crypto::curve_p256) {
    return bn_config_t(DIST(bn)::INT256_POS_0, {bn_filter_t::P256_COEF_FIELD_0});
  } else {
    cb_assert(false);
    return bn_config_t();
  }
}
}  // namespace coinbase::test