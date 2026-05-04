#pragma once

#include <cstddef>
#include <cstring>
#include <istream>

// std::unique_ptr softcopy :D
template <typename T>
class unique_ptr_lite {
private:
  T *ptr;

  unique_ptr_lite &operator=(const unique_ptr_lite &);  // = delete
public:
  unique_ptr_lite(T *ptr) : ptr(ptr) {};
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

  unique_ptr_lite &operator=(const unique_ptr_lite &);  // = delete;
public:
  unique_ptr_lite(T *ptr) : ptr(ptr) {};
  ~unique_ptr_lite() { delete[] ptr; };

  T *get() const { return ptr; };
  T *release() { ptr = nullptr; };

  unique_ptr_lite &operator=(std::nullptr_t np) {
    delete[] ptr;
    ptr = np;
  };

  T &operator[](size_t i) const { return ptr[i]; };
};

class TestCaseResource {
private:
  std::istream &stream;
  const char delimiter;
  const char eol;

  bool is_part_ending_char(int next_char) const {
    return next_char == delimiter || next_char == eol || next_char == EOF;
  }
  size_t get_part_size() const {
    size_t count;
    int next_char;
    while (!is_part_ending_char(next_char = stream.get())) {
      ++count;
    }
    stream.seekg(-count, std::ios::cur);
    return count;
  };

  static void parse_into(int &dest, char *buf, size_t size) {
    dest = atoi(buf);
  }

public:
  TestCaseResource(std::istream &s, char delim, char eol = '\n') : stream(s), delimiter(delim), eol(eol) {};
  ~TestCaseResource() {};

  template <typename T>
  unique_ptr_lite<T> read_next() const;
};

template <typename T>
unique_ptr_lite<T> TestCaseResource::read_next() const {
  size_t part_size = get_part_size();

  char *buf = new char[part_size + 1];
  stream.read(buf, part_size);
  buf[part_size] = '\0';

  T parsed;
  parse_into(parsed, buf, part_size);
  return unique_ptr_lite<T>(&parsed);
}
template <>
unique_ptr_lite<char[]> TestCaseResource::read_next() const {
  size_t part_size = get_part_size();

  char *buf = new char[part_size + 1];
  stream.read(buf, part_size);
  buf[part_size] = '\0';

  return unique_ptr_lite<char[]>(buf);
}
