#pragma once

#include <cbmpc/core/buf.h>
#include <cbmpc/core/error.h>
#include <cbmpc/core/thread.h>
#include <cbmpc/core/utils.h>

namespace coinbase {

struct buf128_t;
struct buf256_t;

class converter_t;

class convertable_t {  // interface
 public:
  virtual void convert(converter_t& converter) = 0;
  virtual ~convertable_t() {}

  class def_t {
   public:
    virtual ~def_t() {}
    virtual convertable_t* create() = 0;
  };

  template <class type, uint64_t code_type>
  class def_entry_t : public def_t {
   public:
    def_entry_t() { factory_t::register_type(this, code_type); }
    virtual convertable_t* create() { return new type(); }
  };

  class factory_t {
   private:
    unordered_map_t<uint64_t, def_t*> map;

   public:
    static void register_type(def_t* def, uint64_t code_type);
    static convertable_t* create(mem_t data, bool convert = true);
    static convertable_t* create(uint64_t code_type);

    template <class type, uint64_t code_type>
    class register_t : public global_init_t<def_entry_t<type, code_type>> {};
  };
};

static global_t<convertable_t::factory_t> g_convertable_factory;

class converter_t {
 public:
  template <typename T>
  static int64_t convert_write(const T& src, byte_ptr out) {
    converter_t converter(true);
    converter.pointer = out;
    converter.convert((T&)src);
    return converter.offset;
  }

  explicit converter_t(bool write);
  explicit converter_t(byte_ptr out);
  explicit converter_t(mem_t src);
  explicit converter_t(cmembig_t src);

  bool is_calc_size() const { return !pointer; }
  bool is_write() const { return write; }
  bool is_error() const { return rv_error != SUCCESS; }
  void set_error();
  void set_error(error_t rv);
  byte_ptr current() const { return pointer + offset; }
  bool at_least(int n) const { return offset + n <= size; }
  void forward(int n) { offset += n; }
  int get_size() const { return (int)(write ? offset : size); }
  int get_offset() const { return (int)offset; }

  void convert(bool& value);
  void convert(uint8_t& value);
  void convert(uint16_t& value);
  void convert(uint32_t& value);
  void convert(uint64_t& value);
  void convert(int8_t& value);
  void convert(int16_t& value);
  void convert(int32_t& value);
  void convert(int64_t& value);
  void convert(std::string& value);

  // void convert_der(buf_t& value);

  template <typename FIRST, typename... LAST>
  void convert(FIRST& first, LAST&... last) {
    convert(first);
    if (!is_error()) convert(last...);
  }

  void convert_len(uint32_t& len);

  template <typename T>
  void convert(T& value) {
    value.convert(*this);
  }
  template <typename T>
  void convert(std::reference_wrapper<T>& value) {
    convert(value.get());
  }

  uint64_t convert_code_type(uint64_t code, uint64_t code2 = 0, uint64_t code3 = 0, uint64_t code4 = 0,
                             uint64_t code5 = 0, uint64_t code6 = 0, uint64_t code7 = 0, uint64_t code8 = 0);

  template <typename T, size_t size>
  void convert(T (&arr)[size]) {
    for (int i = 0; i < size; ++i) {
      convert(arr[i]);
    }
  }

  template <typename T, size_t size>
  void convert(std::array<T, size>& arr) {
    for (int i = 0; i < size; ++i) {
      convert(arr[i]);
    }
  }

  template <typename T>
  void convert(array_view_t<T>& arr) {
    for (int i = 0; i < arr.count; ++i) {
      convert(arr.ptr[i]);
    }
  }

  template <typename T>
  void convert(std::vector<T>& value) {
    if (!write) value.clear();

    uint32_t count = (uint32_t)value.size();
    convert_len(count);

    if (!write) value.resize(count);
    for (uint32_t i = 0; i < count && !is_error(); i++) {
      convert(value[i]);
    }
  }

  template <typename E>
  void convert_enum(E& value) {
    uint32_t tmp = uint32_t(value);
    convert(tmp);
    if (!is_error()) value = E(tmp);
  }

  template <typename... ARGS>
  void convert_flags(ARGS&... args) {
    uint64_t buf = 0;
    if (is_write()) flags_to_buf(buf, 0, args...);
    convert(buf);
    if (!is_error() && !is_write()) flags_from_buf(buf, 0, args...);
  }

  template <typename TKey, typename TItem, typename TInst>
  void convert_with_instance(std::map<TKey, TItem>& value, const TInst& instance) {
    if (!write) value.clear();
    uint32_t count = (uint32_t)value.size();
    convert_len(count);
    auto v = value.begin();
    for (uint32_t i = 0; i < count && !is_error(); i++) {
      if (write) {
        TKey key = v->first;
        convert(key);
        convert(v->second);
        v++;
      } else {
        TKey key;
        convert(key);
        if (is_error()) return;
        auto [ref, is_new] = value.emplace(key, instance);
        if (!is_new) {
          set_error();
          return;
        }
        convert(ref->second);
        if (is_error()) return;
      }
    }
  }

  template <typename TKey, typename TItem>
  void convert(std::map<TKey, TItem>& value) {
    convert_with_instance<TKey, TItem, TItem>(value, TItem());
  }

  void convert(std::vector<bool>& value);
  error_t get_rv() const { return rv_error; }
  static bool is_code_type(mem_t bin, uint64_t code_type) {
    return (bin.size >= 8) && (coinbase::be_get_8(bin.data) == code_type);
  }

  template <typename... T, size_t... I>
  void convert_helper(std::tuple<T...>& tuple, std::index_sequence<I...>) {
    convert(std::get<I>(tuple)...);
  }
  template <typename... T>
  void convert(std::tuple<T...>& tuple) {
    convert_helper(tuple, std::index_sequence_for<T...>{});
  }

  template <typename T1, typename T2>
  void convert(std::pair<T1, T2>& pair) {
    convert(pair.first, pair.second);
  }

 protected:
  error_t rv_error = SUCCESS;
  bool write;
  byte_ptr pointer;
  int64_t offset, size;

 private:
  static void flags_to_buf(uint64_t& buf, int offset) {}

  void flags_from_buf(uint64_t& buf, int offset) {
    if ((buf >> offset) != 0) set_error();
  }

  template <typename FIRST, typename... LAST>
  static void flags_to_buf(uint64_t& buf, int offset, FIRST& first, LAST&... last) {
    if (first) buf |= uint64_t(1) << offset;
    flags_to_buf(buf, offset + 1, last...);
  }

  template <typename FIRST, typename... LAST>
  void flags_from_buf(uint64_t& buf, int offset, FIRST& first, LAST&... last) {
    first = ((buf >> offset) & 1) != 0;
    flags_from_buf(buf, offset + 1, last...);
  }
};

template <typename... ARGS>
buf_t ser(const ARGS&... args) {
  int n;
  {
    converter_t converter(true);
    converter.convert((ARGS&)args...);
    n = converter.get_offset();
  }

  buf_t out(n);

  {
    converter_t converter(out.data());
    converter.convert((ARGS&)args...);
  }

  return out;
}

template <typename... ARGS>
error_t deser(mem_t bin, ARGS&... args) {
  converter_t converter(bin);
  converter.convert(args...);
  return converter.get_rv();
}

template <typename T>
buf_t convert(const T& src) {
  int size = (int)converter_t::convert_write(src, nullptr);
  buf_t result(size);
  converter_t::convert_write(src, result.data());
  return result;
}

template <typename T>
cmembig_t convert_big(const T& src) {
  int64_t size = converter_t::convert_write(src, nullptr);
  cmembig_t result;
  result.data = (byte_ptr)malloc(size);
  result.size = size;
  converter_t::convert_write(src, result.data);
  return result;
}

template <typename T>
error_t convert(T& dst, mem_t src) {
  if (src.size < 0 || (src.size && !src.data)) return coinbase::error(E_BADARG);
  converter_t converter(src);
  converter.convert(dst);
  return converter.get_rv();
}

template <typename T>
error_t convert(T& dst, cmembig_t src) {
  if (src.size < 0 || (src.size && !src.data)) return coinbase::error(E_BADARG);
  converter_t converter(src);
  converter.convert(dst);
  return converter.get_rv();
}

template <typename T, typename... ARGS>
class big_t {
 public:
  big_t(const ARGS&... args) : ptr(new T(args...)) {}
  ~big_t() { delete ptr; }
  operator T&() { return *ptr; }
  operator const T&() const { return *ptr; }
  void convert(converter_t& converter) { converter.convert(*ptr); }

 private:
  T* ptr;
};

}  // namespace coinbase
