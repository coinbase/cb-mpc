#pragma once
#include <cbmpc/crypto/base.h>

struct test_nizk_t {
  uint64_t aux = 0;
  buf_t sid = coinbase::crypto::gen_random(16);

  virtual void setup() = 0;
  virtual void prove() = 0;
  virtual error_t verify() = 0;
  virtual uint64_t proof_size() = 0;
};
