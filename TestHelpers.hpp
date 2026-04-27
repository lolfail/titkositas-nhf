#pragma once
#include <cstddef>
#include <type_traits>

#include "gtest_lite.h"

#include "Cipher.h"
#include "CipherAlgorithm.h"
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
using void_return_if =
  typename std::enable_if<to_enable, void>::type;

template <typename TAlgorithm>
void_return_if<!is_cstr<TAlgorithm>()>
init_polymorphic_object(Cipher &target, int key, const char *cstr) {
  target = Cipher(new TAlgorithm(key));
  target = cstr;
}
template <typename TAlgorithm>
void_return_if<is_cstr<TAlgorithm>()>
init_polymorphic_object(const char *&target, int key, const char *cstr) {
  target = cstr;
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
  init_polymorphic_object<Tlhs>(lhs, key_lhs, init_lhs);
  CStringElseCipher<is_rhs_cstr> rhs;
  init_polymorphic_object<Trhs>(rhs, key_rhs, init_rhs);

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
  init_polymorphic_object<Tlhs>(lhs, key_lhs, init_lhs);
  CStringElseCipher<is_rhs_cstr> rhs;
  init_polymorphic_object<Trhs>(rhs, key_rhs, init_rhs);

  bool expected_eq = strcmp(init_lhs, init_rhs) == 0;
  if (eq_op) {
    EXPECT_EQ(expected_eq, (lhs == rhs))
      << "Library handled equality differently than expected!";
  } else {
    EXPECT_EQ(!expected_eq, (lhs != rhs))
      << "Library handled inequality differently than expected!";
  }
}

inline ptrdiff_t iterator_position_from_start(const Cipher &parent, const Cipher::const_iterator &it) {
  return it - parent.begin();
}

struct IteratorOffsettingValues {
  size_t initial_shift;
  ptrdiff_t returned_expected_shift;
  ptrdiff_t original_expected_shift;
  IteratorOffsettingValues(size_t init, ptrdiff_t ret, ptrdiff_t orig)
    : initial_shift(init), returned_expected_shift(ret), original_expected_shift(orig) {}
};
template <typename TAlgorithm, typename TAlgorithmParam, typename F>
void test_iterator_offsetting(F iterator_operator, TAlgorithmParam key, const char *cstr, const IteratorOffsettingValues &iter_offsets) {
  Cipher cipher = allocate_initialized_cipher<TAlgorithm>(key, cstr);
  Cipher::const_iterator it_original = cipher.begin() + iter_offsets.initial_shift;

  Cipher::const_iterator it_return = iterator_operator(it_original);

  ptrdiff_t original_delta = iterator_position_from_start(cipher, it_original);
  EXPECT_EQ(original_delta, iter_offsets.original_expected_shift)
    << "The original iterator's position does not match that of expected!";

  ptrdiff_t returned_delta = iterator_position_from_start(cipher, it_return);
  EXPECT_EQ(returned_delta, iter_offsets.returned_expected_shift)
    << "The returned iterator's position does not match that of expected!";
}
