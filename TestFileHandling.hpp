#pragma once

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ios>
#include <iostream>
#include <istream>
#include <ostream>
#include <stdexcept>

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
std::ostream &operator<<(std::ostream &os, unique_ptr_lite<T> &ptr) {
  os << *ptr;
  return os;
}
template <>
std::ostream &operator<<(std::ostream &os, unique_ptr_lite<char[]> &ptr) {
  os << ptr.get();
  return os;
}

class TestCaseResource {
private:
  std::istream &stream;

  const char *delimiter;
  const char *eol;
  size_t delimiter_len;
  size_t eol_len;

  enum class object_at_cursor {
    file_ending,
    line_ending,
    delimiter,
    other
  };
  size_t get_count_until_data(object_at_cursor under_cursor) const {
    switch (under_cursor) {
      case object_at_cursor::other:
        return 0;
      case object_at_cursor::file_ending:
        return 0;
      case object_at_cursor::line_ending:
        return eol_len;
      case object_at_cursor::delimiter:
        return delimiter_len;
      default:
        throw std::range_error("Matched unexpected object under cursor!");
    }
  }

  bool is_at_beginning(const char *haystack, const char *needle) const {
    const char *found_at_ptr = strstr(haystack, needle);
    if (found_at_ptr == NULL) return false;
    ptrdiff_t found_at_idx = found_at_ptr - haystack;
    return found_at_idx == 0;
  }

  object_at_cursor get_object_under_cursor() const {
    size_t max_len = delimiter_len > eol_len ? delimiter_len : eol_len;

    char *buf = new char[max_len + 1];
    stream.read(buf, max_len);
    size_t read_count = stream.gcount();
    stream.seekg(-read_count, std::ios::cur);
    buf[read_count] = '\0';

    object_at_cursor under_cursor;

    bool eof_at_beginning = read_count == 1 && stream.eof();
    if (eof_at_beginning)
      under_cursor = object_at_cursor::file_ending;
    else if (is_at_beginning(buf, delimiter))
      under_cursor = object_at_cursor::delimiter;
    else if (is_at_beginning(buf, eol))
      under_cursor = object_at_cursor::line_ending;
    else
      under_cursor = object_at_cursor::other;

    delete[] buf;
    return under_cursor;
  }

  size_t get_part_size() const {
    size_t moved = 0;

    while (get_object_under_cursor() == object_at_cursor::other) {
      stream.seekg(1, std::ios::cur);
      ++moved;
    };

    stream.seekg(-moved, std::ios::cur);
    return moved;
  };

  void move_to_start_of_data() const {
    auto object_under_cursor = get_object_under_cursor();
    size_t to_move = get_count_until_data(object_under_cursor);
    stream.seekg(to_move, std::ios::cur);
  }

  static void parse_into(int *dest, char *buf, size_t size) {
    *dest = atoi(buf);
  }
  static void parse_into(size_t *dest, char *buf, size_t size) {
    sscanf(buf, "%zu", &size);
  }

public:
  TestCaseResource(std::istream &s, const char *delim = ", ", const char *e = "\n") : stream(s), delimiter(delim), eol(e) {
    eol_len = strlen(eol);
    delimiter_len = strlen(delimiter);
  }
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

  T *parsed = new T();
  parse_into(parsed, buf, part_size);
  delete[] buf;

  move_to_start_of_data();
  return unique_ptr_lite<T>(parsed);
}
template <>
unique_ptr_lite<char[]> TestCaseResource::read_next() const {
  size_t part_size = get_part_size();

  char *buf = new char[part_size + 1];
  stream.read(buf, part_size);
  buf[part_size] = '\0';

  move_to_start_of_data();
  return unique_ptr_lite<char[]>(buf);
}
