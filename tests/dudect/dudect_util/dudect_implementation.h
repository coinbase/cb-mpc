#include <cbmpc/crypto/base.h>

namespace coinbase::dudect {

#include "dudect.h"
#define DUDECT_IMPLEMENTATION

inline std::function<uint8_t(uint8_t*)> active_funct;
inline std::function<void(uint8_t, uint16_t)> input_generator;
inline bn_t denormalize(bn_t x, mod_t mod_q) {
  MODULO(mod_q) x += 0;
  return x;
}
inline uint16_t get_start_idx(uint8_t* data, uint8_t number_ops) { return ((data[0] << 8) | data[1]) * number_ops; }
inline uint8_t do_one_computation(uint8_t* data) {
  active_funct(data);
  return 0;
}
/* called once per number_measurements */
inline void prepare_inputs(dudect_config_t* c, uint8_t* input_data, uint8_t* classes) {
  for (size_t i = 0; i < c->number_measurements; i++) {
    /* it is important to randomize the class sequence */
    classes[i] = randombit();
    uint16_t val = (uint16_t)i;

    // Allow for larger range of measurement values
    memset(input_data + (size_t)i * c->chunk_size, (val >> 8) & 0xff, 1);
    memset(input_data + (size_t)i * c->chunk_size + 1, val & 0xff, 1);
    input_generator(classes[i], val);
  }
}
}  // namespace coinbase::dudect