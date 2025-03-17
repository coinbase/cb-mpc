#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tag_cmem_t {
  uint8_t* data;
  int size;
} cmem_t;

typedef struct tag_cmems_t {
  int count;
  uint8_t* data;
  int* sizes;
} cmems_t;

typedef struct tag_cmem_big_t {
  uint8_t* data;
  int64_t size;
} cmembig_t;

#ifdef __cplusplus
}
#endif
