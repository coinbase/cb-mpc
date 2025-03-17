#pragma once

#pragma GCC visibility push(hidden)

namespace coinbase::crypto {

template <typename T>
class scoped_ptr_t {
 public:
  typedef T* ptr_type;

  scoped_ptr_t() : ptr(nullptr) {}
  scoped_ptr_t(T* _ptr) : ptr(_ptr) {}
  scoped_ptr_t(scoped_ptr_t&& src) : ptr(src.ptr) { src.ptr = nullptr; }
  scoped_ptr_t(const scoped_ptr_t& src) : ptr(src.ptr ? copy(src.ptr) : nullptr) {}

  ~scoped_ptr_t() {
    if (ptr) free();
  }
  void attach(T* _ptr) { ptr = _ptr; }
  T* detach() {
    T* old = ptr;
    ptr = nullptr;
    return old;
  }
  void free() {
    if (ptr) free(ptr);
    ptr = nullptr;
  }
  operator T*() const { return ptr; }
  T* operator->() const { return ptr; }
  operator bool() const { return ptr != nullptr; }
  bool operator!() const { return ptr == nullptr; }
  T* pointer() const { return ptr; }
  bool valid() const { return ptr != nullptr; }

  scoped_ptr_t& operator=(scoped_ptr_t&& src) {
    if (this != &src) {
      free();
      ptr = src.ptr;
      src.ptr = nullptr;
    }
    return *this;
  }

  scoped_ptr_t& operator=(const scoped_ptr_t& src) {
    if (this != &src) {
      free();
      if (src.ptr) ptr = copy(src.ptr);
    }
    return *this;
  }

 protected:
  T* ptr;
  static void free(T* ptr);
  static T* copy(T* ptr);
};

}  // namespace coinbase::crypto
