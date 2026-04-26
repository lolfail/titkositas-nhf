#pragma once
#include <type_traits>

#include "gtest_lite.h"

#include "CipherAlgorithm.h"
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

template <typename TAlgorithm>
constexpr bool is_cstr() {
  return std::is_same<TAlgorithm, const char*>::value;
}

// so logic is built into typesystem
template <bool to_enable>
using algoptr_return_if =
  typename std::enable_if<to_enable, Algorithm*>::type;

template <typename TAlgorithm>
algoptr_return_if<!is_cstr<TAlgorithm>()>
init_object_return_alloc(Cipher &target, int key, const char *cstr) {
  Algorithm *ptr = new TAlgorithm(key);
  target = Cipher(ptr);
  target = cstr;
  return ptr;
}
template <typename TAlgorithm>
algoptr_return_if<is_cstr<TAlgorithm>()>
init_object_return_alloc(const char *&target, int key, const char *cstr) {
  target = cstr;
  return nullptr;
}

template <bool is_cstr>
using CStringElseCipher =
  typename std::conditional<is_cstr, const char*, Cipher>::type;

template <bool is_append, typename Tlhs, typename Trhs>
void test_concatenation(const char *init_lhs, const int key_lhs, const char *init_rhs, const int key_rhs) {
  constexpr bool is_lhs_cstr = is_cstr<Tlhs>();
  constexpr bool is_rhs_cstr = is_cstr<Trhs>();

  static_assert(!(is_append && is_lhs_cstr), "Operation: const char* += ... is not defined!");
  static_assert(!(is_lhs_cstr && is_rhs_cstr), "Operation: const char* + const char* is not defined!");

  CStringElseCipher<is_lhs_cstr> lhs;
  Algorithm *lhs_algo = init_object_return_alloc<Tlhs>(lhs, key_lhs, init_lhs); // defer delete
  CStringElseCipher<is_rhs_cstr> rhs;
  Algorithm *rhs_algo = init_object_return_alloc<Trhs>(rhs, key_rhs, init_rhs); // defer delete

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

  delete rhs_algo;
  delete lhs_algo;
}


template <typename TAlgorithm, typename TAlgorithmParam>
Cipher allocate_initialized_cipher(TAlgorithmParam init_param, const char *cstr) {
  Cipher initialized_cipher(new TAlgorithm(init_param));
  initialized_cipher = cstr;
  return initialized_cipher;
}


template <bool eq_op, typename Tlhs, typename Trhs>
void test_logic_operation(const char *init_lhs, const int key_lhs, const char *init_rhs, const int key_rhs) {
  constexpr bool is_lhs_cstr = is_cstr<Tlhs>();
  constexpr bool is_rhs_cstr = is_cstr<Trhs>();

  static_assert(!(is_lhs_cstr && is_rhs_cstr), "Test case: const char* [op] const char* does not test the library!");

  CStringElseCipher<is_lhs_cstr> lhs;
  Algorithm *lhs_algo = init_object_return_alloc<Tlhs>(lhs, key_lhs, init_lhs); // defer delete
  CStringElseCipher<is_rhs_cstr> rhs;
  Algorithm *rhs_algo = init_object_return_alloc<Trhs>(rhs, key_rhs, init_rhs); // defer delete

  bool expected_eq = strcmp(init_lhs, init_rhs) == 0;
  if (eq_op) {
    EXPECT_EQ(expected_eq, (lhs == rhs))
      << "Library handled equality differently than expected!";
  } else {
    EXPECT_EQ(!expected_eq, (lhs != rhs))
      << "Library handled inequality differently than expected!";
  }

  delete rhs_algo;
  delete lhs_algo;
}
