#pragma once

namespace coinbase {

template <class T>
class global_t {
 public:
  global_t() noexcept(true) { change_ref_count(+1); }
  ~global_t() {
    if (change_ref_count(-1)) return;
    T* ptr = instance_ptr(false);
    if (ptr) ptr->~T();
  }
  T& instance() { return *instance_ptr(true); }

 private:
  static T* instance_ptr(bool force) {
    static std::once_flag once;
    static bool initialized = false;
    if (!force && !initialized) return nullptr;

    static unsigned char __attribute__((aligned(16))) buf[sizeof(T)];

    std::call_once(once, []() {
      new ((T*)buf) T();
      initialized = true;
    });
    return (T*)buf;
  }
  static int change_ref_count(int x) {
    static int ref_count = 0;
    return ref_count += x;
  }
};

template <class T>
class global_init_t : public global_t<T> {
 public:
  global_init_t() : global_t<T>() { global_t<T>::instance(); }
};

}  // namespace coinbase
