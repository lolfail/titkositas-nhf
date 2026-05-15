#pragma once

#include <ostream>
#include <cstddef>

// std::unique_ptr softcopy :D
template <typename T>
class unique_ptr_lite {
private:
  T *ptr;

  unique_ptr_lite(const unique_ptr_lite &);             // = delete
  unique_ptr_lite &operator=(const unique_ptr_lite &);  // = delete
public:
  explicit unique_ptr_lite(T *ptr) : ptr(ptr) {};
  unique_ptr_lite(unique_ptr_lite &&other_unique_ptr) {
    ptr = other_unique_ptr.ptr;
    other_unique_ptr.ptr = nullptr;
  }
  ~unique_ptr_lite() { delete ptr; };

  T *get() const { return ptr; };
  T *release() { ptr = nullptr; };

  unique_ptr_lite &operator=(std::nullptr_t np) {
    delete ptr;
    ptr = np;
  };

  T &operator*() const { return *ptr; };
  T *operator->() const { return ptr; };
};

template <typename T>
class unique_ptr_lite<T[]> {
private:
  T *ptr;

  unique_ptr_lite(const unique_ptr_lite &);             // = delete
  unique_ptr_lite &operator=(const unique_ptr_lite &);  // = delete;
public:
  explicit unique_ptr_lite(T *ptr) : ptr(ptr) {};
  unique_ptr_lite(unique_ptr_lite &&other_unique_ptr) {
    ptr = other_unique_ptr.ptr;
    other_unique_ptr.ptr = nullptr;
  }
  ~unique_ptr_lite() { delete[] ptr; };

  T *get() const { return ptr; };
  T *release() { ptr = nullptr; };

  unique_ptr_lite &operator=(std::nullptr_t np) {
    delete[] ptr;
    ptr = np;
  };

  T &operator[](size_t i) const { return ptr[i]; };
};

template <typename T>
inline std::ostream &operator<<(std::ostream &os, unique_ptr_lite<T> &ptr) {
  os << *ptr;
  return os;
}
template <>
inline std::ostream &operator<<(std::ostream &os, unique_ptr_lite<char[]> &ptr) {
  os << ptr.get();
  return os;
}
