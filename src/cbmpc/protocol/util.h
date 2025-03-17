#pragma once

#include <cbmpc/crypto/base.h>

template <typename T, typename LAMBDA>
static T SUM(T zero, int n, LAMBDA lambda) {
  T sum = zero;
  for (int index = 0; index < n; index++) {
    lambda(sum, index);
  }
  return sum;
}

template <typename T, typename LAMBDA>
static T SUM(int n, LAMBDA lambda) {
  return SUM(T(), n, lambda);
}

template <typename T>
static T SUM(const std::vector<T>& v) {
  T s = v[0];
  for (int i = 1; i < int(v.size()); i++) s += v[i];
  return s;
}

template <typename T>
static T SUM(const std::vector<std::reference_wrapper<T>>& v) {
  T s = v[0].get();
  for (int i = 1; i < int(v.size()); i++) s += v[i].get();
  return s;
}

static bn_t SUM(const std::vector<bn_t>& v, const mod_t& q) {
  bn_t s = 0;
  for (int i = 0; i < int(v.size()); i++) MODULO(q) s += v[i];
  return s;
}

static bn_t SUM(const std::vector<std::reference_wrapper<bn_t>>& v, const mod_t& q) {
  bn_t s = 0;
  for (int i = 0; i < int(v.size()); i++) MODULO(q) s += v[i];
  return s;
}

// Helper function that applies 'f' to each element in 'tup' and returns a new tuple of the results.
template <class F, typename... Args, std::size_t... I>
auto map_args_to_tuple_impl(F f, std::tuple<Args...>& tup, std::index_sequence<I...>) {
  return std::make_tuple((f(std::get<I>(tup)))...);
}

// map_args_to_tuple applies function 'f' to every argument in 'args...' and returns them as a tuple.
template <class F, typename... Args>
auto map_args_to_tuple(F f, Args&&... args) {
  std::tuple<Args...> tup(std::forward<Args>(args)...);
  return map_args_to_tuple_impl(f, tup, std::index_sequence_for<Args...>{});
}

// Returns a vector of reference_wrappers<T> obtained from a vector of shared_ptr<T>.
template <typename T>
std::vector<std::reference_wrapper<T>> extract_refs(const std::vector<std::shared_ptr<T>>& shared_ptr_vec) {
  std::vector<std::reference_wrapper<T>> ref_vec;
  ref_vec.reserve(shared_ptr_vec.size());
  for (const auto& ptr : shared_ptr_vec) {
    ref_vec.push_back(*ptr);
  }
  return ref_vec;
}

// Returns a vector of T by dereferencing each shared_ptr<T> in the input vector.
template <typename T>
std::vector<T> extract_values(const std::vector<std::shared_ptr<T>>& shared_ptr_vec) {
  std::vector<T> value_vec;
  value_vec.reserve(shared_ptr_vec.size());
  for (const auto& ptr : shared_ptr_vec) {
    value_vec.push_back(*ptr);
  }
  return value_vec;
}
