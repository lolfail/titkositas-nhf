#pragma once
#include <type_traits>

#include "gtest_lite.h"

#include "Cipher.h"
#include "XORCipher.h"

inline char* allocate_concatenated(const char *lhs, const char *rhs) {
  char *concat_buf = new char[strlen(lhs) + strlen(rhs) + 1];
  strcpy(concat_buf, lhs);
  strcat(concat_buf, rhs);
  return concat_buf;
};

inline const char* get_cstr_value(const Cipher &cipher) { return cipher.c_str(); }
inline const char* get_cstr_value(const char *cstr) { return cstr; }

inline void initialize_object(Cipher &target, int key, const char *cstr) {
  target = Cipher(new XORCipher(key));
  target = cstr;
}
inline void initialize_object(const char *&target, int key, const char *cstr) {
  target = cstr;
}

template <bool is_object>
using CipherOrCString =
  typename std::conditional<is_object, Cipher, const char*>::type;


template <bool is_append, bool is_lhs_object, bool is_rhs_object>
void test_concatenation(const char *init_lhs, const int key_lhs, const char *init_rhs, const int key_rhs) {
  static_assert(!(is_append && !is_lhs_object), "Operation: const char* += ... is not defined!");
  static_assert(is_lhs_object || is_rhs_object, "Operation: const char* + const char* is not defined!");

  CipherOrCString<is_lhs_object> lhs;
  initialize_object(lhs, key_lhs, init_lhs);

  CipherOrCString<is_rhs_object> rhs;
  initialize_object(rhs, key_rhs, init_rhs);

  Cipher resulting_cipher;
  if (is_append)
    resulting_cipher = (lhs += rhs);
  else
    resulting_cipher = lhs + rhs;

  if (!is_append) {
    EXPECT_STREQ(get_cstr_value(lhs), init_lhs)
      << "LHS's stored data was changed!";
  }

  EXPECT_STREQ(get_cstr_value(rhs), init_rhs)
    << "RHS's stored data was changed!";

  char *expected_result = allocate_concatenated(init_lhs, init_rhs);
  EXPECT_STREQ(resulting_cipher.c_str(), expected_result)
    << "Result differs from expected!";
  delete[] expected_result;
}

template <class TAlgorithm, typename TAlgorithmParam>
Cipher allocate_initialized_cipher(TAlgorithmParam init_param, const char *cstr) {
  Cipher initialized_cipher(new TAlgorithm(init_param));
  initialized_cipher = cstr;
  return initialized_cipher;
}
