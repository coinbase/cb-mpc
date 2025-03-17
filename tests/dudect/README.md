# Constant-Time Testing

## Creating a Dudect Test

Our constant-time testing is based on the [dudect library](https://github.com/oreparaz/dudect).
On top of that, we have a wrapper (`dudect_util/dudect_implementation.h`), which allows tests to be written with a larger variety of inputs and more complex target functions. In order to run a dudect test in cb-mpc, the function pointers of `input_generator` and `active_funct` must be defined within the unit test.

The general basis of input generation involves the random creation of a fixed and variable input class. The code below is an example from `dudect_mod_test.cpp`.

```bash
bn_t bn_arr[NUMBER_OPERANDS * NUMBER_MEASUREMENTS];
bn_t base_bn_a;
bn_t base_bn_b;
ecurve_t curve;
mod_t q;
void generate_bn_array(uint8_t c, uint16_t idx)
{
  // Creates random value for non-control group, sets fixed value for control group
  uint16_t start_idx = NUMBER_OPERANDS * idx;
  if (c == 1)
  {
    bn_arr[start_idx] = denormalize(bn_t::rand(q), q);
    bn_arr[start_idx + 1] = denormalize(bn_t::rand(q), q);
  }
  else
  {
    bn_arr[start_idx] = denormalize(base_bn_a, q);
    bn_arr[start_idx + 1] = denormalize(base_bn_b, q);
  }
}
```

In this case, additional preparation is used to ensure that the size of inputs is constant for the target function. This function is passed into `input_generator`, which is used in the dudect wrapper to create new measurements for each cycle of `NUMBER_MEASUREMENTS`. The `denormalize` function is used to pad the leading zero to have all inputs with the same length.

When defining a target function for `active_funct`, it's important to isolate the operation that is intended to be measured as much as possible. The example below shows an example of modular addition which only has additional code to access the index of the prepared data.

```bash
uint8_t test_mod_add(uint8_t* data)
{
  uint16_t start_idx = get_start_idx(data, NUMBER_OPERANDS);
  MODULO(q) { bn_arr[start_idx] + bn_arr[start_idx + 1]; }
  return 0;
}
```

With both function pointers assigned, a specialized dudect test implementation can be created through a function such as `run_dudect_leakage_test`, which has slightly different variations for each test. The most relevant of these differences comes in the form of the value for `measurement_threshold` which has conditions to prevent a leakage test from overflowing and running indefinitely.

## Running Dudect

In order to run a dudect test, the command `make dudect` is used along with an optional filter on the tests run.

```bash
make dudect filter=DUDECT_CT_BN_CORE
```
