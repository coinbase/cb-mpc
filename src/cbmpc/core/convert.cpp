#include "convert.h"

namespace coinbase {

void converter_t::convert(bool& value) {
  uint8_t v = value ? 1 : 0;
  convert(v);
  if (!is_error() && !write) value = v != 0;
}

void converter_t::convert(uint8_t& value) {
  if (write) {
    if (pointer) *current() = value;
  } else {
    if (is_error() || !at_least(1)) {
      set_error();
      return;
    }
    value = *current();
  }
  forward(1);
}

void converter_t::convert(int8_t& value) {
  uint8_t v = value;
  convert(v);
  if (!is_error() && !write) value = v;
}

void converter_t::convert(uint16_t& value) {
  if (write) {
    if (pointer) coinbase::be_set_2(current(), value);
  } else {
    if (is_error() || !at_least(2)) {
      set_error();
      return;
    }
    value = coinbase::be_get_2(current());
  }
  forward(2);
}

void converter_t::convert(int16_t& value) {
  uint16_t v = value;
  convert(v);
  if (!is_error() && !write) value = v;
}

void converter_t::convert(uint32_t& value) {
  if (write) {
    if (pointer) coinbase::be_set_4(current(), value);
  } else {
    if (is_error() || !at_least(4)) {
      set_error();
      return;
    }
    value = coinbase::be_get_4(current());
  }
  forward(4);
}

void converter_t::convert(int32_t& value) {
  uint32_t v = value;
  convert(v);
  if (!is_error() && !write) value = v;
}

void converter_t::convert(uint64_t& value) {
  if (write) {
    if (pointer) coinbase::be_set_8(current(), value);
  } else {
    if (is_error() || !at_least(8)) {
      set_error();
      return;
    }
    value = coinbase::be_get_8(current());
  }
  forward(8);
}

void converter_t::convert(int64_t& value) {
  uint64_t v = value;
  convert(v);
  if (!is_error() && !write) value = v;
}

void converter_t::convert(std::string& value) {
  if (write) {
    // serialization length validation
    cb_assert(value.length() <= SHRT_MAX);
  }

  short value_size = (short)value.length();
  convert(value_size);

  if (write) {
    if (pointer) memmove(current(), &value[0], value_size);
  } else {
    if (value_size < 0) {
      set_error();
      return;
    }  // deserialization length validation

    if (is_error() || !at_least(value_size)) {
      set_error();
      return;
    }
    value.resize(value_size);
    memmove(&value[0], current(), value_size);
  }
  forward(value_size);
}

converter_t::converter_t(bool _write) : write(_write), rv_error(0), pointer(nullptr), offset(0), size(0) {}

converter_t::converter_t(byte_ptr out) : write(true), rv_error(0), pointer(out), offset(0), size(0) {}

converter_t::converter_t(mem_t src) : write(false), rv_error(0), pointer(src.data), offset(0), size(src.size) {}

converter_t::converter_t(cmembig_t src) : write(false), rv_error(0), pointer(src.data), offset(0), size(src.size) {}

void converter_t::set_error() {
  if (rv_error) return;
  rv_error = coinbase::error(E_FORMAT, "Converter error" + std::string(write ? "(write)" : "(read)"));
}

void converter_t::set_error(error_t rv) {
  if (rv_error) return;
  rv_error = coinbase::error(rv);
}

void converter_t::convert_len(uint32_t& len) {
  byte_t b = 0;
  if (write) {
    cb_assert(len <= 0x1fffffff);
    if (len <= 0x7f) {
      b = byte_t(len);
      convert(b);
      return;
    }
    if (len <= 0x3fff) {
      b = byte_t(len >> 8) | 0x80;
      convert(b);
      b = byte_t(len);
      convert(b);
      return;
    }
    if (len <= 0x1fffff) {
      b = byte_t(len >> 16) | 0xc0;
      convert(b);
      b = byte_t(len >> 8);
      convert(b);
      b = byte_t(len);
      convert(b);
      return;
    }
    b = byte_t(len >> 24) | 0xe0;
    convert(b);
    b = byte_t(len >> 16);
    convert(b);
    b = byte_t(len >> 8);
    convert(b);
    b = byte_t(len);
    convert(b);
  } else {
    convert(b);
    if (is_error()) {
      len = 0;
      return;
    }
    if ((b & 0x80) == 0) {
      len = b;
      return;
    }
    if ((b & 0x40) == 0) {
      len = b & 0x3f;
      convert(b);
      len = (len << 8) | b;
      if (is_error()) len = 0;
      return;
    }
    if ((b & 0x20) == 0) {
      len = b & 0x1f;
      convert(b);
      len = (len << 8) | b;
      convert(b);
      len = (len << 8) | b;
      if (is_error()) len = 0;
      return;
    }
    len = b & 0x1f;
    convert(b);
    len = (len << 8) | b;
    convert(b);
    len = (len << 8) | b;
    convert(b);
    len = (len << 8) | b;
    if (is_error()) len = 0;
  }
}

void converter_t::convert(std::vector<bool>& value) {
  if (!write) value.clear();

  short count = (short)value.size();
  convert(count);

  if (!write) value.resize(count);
  for (short i = 0; i < count && !is_error(); i++) {
    bool v = value[i];
    convert(v);
    value[i] = v;
  }
}

void convertable_t::factory_t::register_type(def_t* def, uint64_t code_type) {
  g_convertable_factory.instance().map[code_type] = def;
}

convertable_t* convertable_t::factory_t::create(uint64_t code_type) {
  const auto& map = g_convertable_factory.instance().map;
  const auto i = map.find(code_type);
  if (i == map.end()) return nullptr;
  return i->second->create();
}

convertable_t* convertable_t::factory_t::create(mem_t mem, bool convert) {
  if (mem.size < sizeof(uint64_t)) return nullptr;

  uint64_t code_type = be_get_8(mem.data);
  convertable_t* obj = create(code_type);
  if (!convert) return obj;

  if (!obj) return nullptr;

  converter_t converter(mem);
  obj->convert(converter);
  if (!converter.is_error()) return obj;

  delete obj;
  return nullptr;
}

uint64_t converter_t::convert_code_type(uint64_t code, uint64_t code2, uint64_t code3, uint64_t code4, uint64_t code5,
                                        uint64_t code6, uint64_t code7, uint64_t code8) {
  uint64_t value = code;
  convert(value);
  if (is_error()) return 0;
  if (!write) {
    if (value == code) return value;
    if (code2 && value == code2) return value;
    if (code3 && value == code3) return value;
    if (code4 && value == code4) return value;
    if (code5 && value == code5) return value;
    if (code6 && value == code6) return value;
    if (code7 && value == code7) return value;
    if (code8 && value == code8) return value;
    set_error();
    return 0;
  }
  return value;
}

}  // namespace coinbase
