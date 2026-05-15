#pragma once

#include <cstddef>
#include <cstring>
#include <istream>

#include "UniquePointerLite.hpp"

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
  size_t get_count_until_data(object_at_cursor under_cursor) const;

  bool is_at_beginning(const char *haystack, const char *needle) const;

  object_at_cursor get_object_under_cursor() const;

  size_t get_part_size() const;

  void move_to_start_of_data() const;
  void skip_empty_newlines() const;

  static void parse_into(int *dest, char *buf, size_t size) {
    *dest = atoi(buf);
  }
  static void parse_into(size_t *dest, char *buf, size_t size) {
    sscanf(buf, "%zu", dest);
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
