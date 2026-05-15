#include "IOUtils.hpp"

#include <cstring>

#include "UniquePointerLite.hpp"
#include "memtrace.h"

size_t TestCaseResource::get_count_until_data(object_at_cursor under_cursor) const {
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

bool TestCaseResource::is_at_beginning(const char *haystack, const char *needle) const {
  const char *found_at_ptr = strstr(haystack, needle);
  if (found_at_ptr == NULL) return false;
  ptrdiff_t found_at_idx = found_at_ptr - haystack;
  return found_at_idx == 0;
}

TestCaseResource::object_at_cursor TestCaseResource::get_object_under_cursor() const {
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

size_t TestCaseResource::get_part_size() const {
  size_t moved = 0;

  while (get_object_under_cursor() == object_at_cursor::other) {
    stream.seekg(1, std::ios::cur);
    ++moved;
  };

  stream.seekg(-moved, std::ios::cur);
  return moved;
};

void TestCaseResource::move_to_start_of_data() const {
  auto object_under_cursor = get_object_under_cursor();
  size_t to_move = get_count_until_data(object_under_cursor);
  stream.seekg(to_move, std::ios::cur);
}

void TestCaseResource::skip_empty_newlines() const {
  while (get_object_under_cursor() == object_at_cursor::line_ending)
    move_to_start_of_data();
}

template <typename T>
unique_ptr_lite<T> TestCaseResource::read_next() const {
  skip_empty_newlines();

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
// generate versions:
template unique_ptr_lite<int> TestCaseResource::read_next<int>() const;
template unique_ptr_lite<size_t> TestCaseResource::read_next<size_t>() const;

template <>
unique_ptr_lite<char[]> TestCaseResource::read_next() const {
  skip_empty_newlines();

  size_t part_size = get_part_size();

  char *buf = new char[part_size + 1];
  stream.read(buf, part_size);
  buf[part_size] = '\0';

  move_to_start_of_data();
  return unique_ptr_lite<char[]>(buf);
}
