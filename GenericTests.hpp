#pragma once

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <type_traits>

#include "Cipher.h"
#include "IOUtils.hpp"
#include "gtest_lite.h"
#include "memtrace.h"

namespace testing {

  inline bool radius_of_index_within_cstr_contents(char *cstr, size_t idx, size_t radius) {
    size_t len = strlen(cstr);
    bool idx_goes_negative = radius > idx;
    bool idx_overflows_cstring = (idx + radius) > len;  // NUL chr is not allowed
    return !(idx_goes_negative || idx_overflows_cstring);
  }

  inline char *allocate_concatenated(const char *lhs, const char *rhs) {
    char *concat_buf = new char[strlen(lhs) + strlen(rhs) + 1];
    strcpy(concat_buf, lhs);
    strcat(concat_buf, rhs);
    return concat_buf;
  };

  template <typename TAlgorithm, typename TAlgorithmParam>
  Cipher allocate_initialized_cipher(TAlgorithmParam init_param, const char *cstr) {
    Cipher initialized_cipher(new TAlgorithm(init_param));
    initialized_cipher = cstr;
    return initialized_cipher;
  }

  template <bool is_cstr>
  using CStringElseCipher =
    typename std::conditional<is_cstr, const char *, Cipher>::type;

  inline const char *get_cstr_value(const Cipher &cipher) { return cipher.c_str(); }
  inline const char *get_cstr_value(const char *cstr) { return cstr; }

  // so logic is built into typesystem
  template <bool to_enable, typename Treturn>
  using return_if =
    typename std::enable_if<to_enable, Treturn>::type;

  template <typename TAlgorithm>
  constexpr bool is_cstr() {
    return std::is_same<TAlgorithm, const char *>::value;
  }

  template <typename TAlgorithm>
  return_if<!is_cstr<TAlgorithm>(), void>
  init_polymorphic_object(Cipher &target, int key, const char *cstr) {
    target = Cipher(new TAlgorithm(key));
    target = cstr;
  }
  template <typename TAlgorithm>
  return_if<is_cstr<TAlgorithm>(), void>
  init_polymorphic_object(const char *&target, int key, const char *cstr) {
    target = cstr;
  }

  namespace indexing {
    void call_tests(TestCaseResource &);
  } // namespace indexing

  namespace assignation {
    void call_tests(TestCaseResource &);

    template <typename TDestAlgorithm, typename TSrcAlgorithm, typename TDestAlgorithmParam, typename TSrcAlgorithmParam>
    void check(TDestAlgorithmParam dest_key, TSrcAlgorithmParam src_key, const char *cstr) {
      constexpr bool is_dest_cstr = is_cstr<TDestAlgorithm>();
      constexpr bool is_src_cstr = is_cstr<TSrcAlgorithm>();

      static_assert(!is_dest_cstr, "Assignation: const char* = ... is not defined!");

      Cipher destination(new TDestAlgorithm(dest_key));

      CStringElseCipher<is_src_cstr> source;
      init_polymorphic_object<TSrcAlgorithm>(source, src_key, cstr);

      destination = source;
      EXPECT_STREQ(destination.c_str(), get_cstr_value(source))
        << "Assigned object's value differs from that of source!";
    }
  }  // namespace assignation

  namespace concatenation {
    void call_tests(TestCaseResource &);

    template <bool is_append, typename Tlhs, typename Trhs>
    return_if<is_append, Cipher>
    append_else_concat(Tlhs &lhs, Trhs &rhs) {
      return lhs += rhs;
    }
    template <bool is_append, typename Tlhs, typename Trhs>
    return_if<!is_append, Cipher>
    append_else_concat(Tlhs &lhs, Trhs &rhs) {
      return lhs + rhs;
    }

    template <bool is_append, typename Tlhs, typename Trhs>
    void check(const char *init_lhs, const int key_lhs, const char *init_rhs, const int key_rhs) {
      constexpr bool is_lhs_cstr = is_cstr<Tlhs>();
      constexpr bool is_rhs_cstr = is_cstr<Trhs>();

      static_assert(!(is_append && is_lhs_cstr), "Operation: const char* += ... is not defined!");
      static_assert(!(is_lhs_cstr && is_rhs_cstr), "Operation: const char* + const char* is not defined!");

      CStringElseCipher<is_lhs_cstr> lhs;
      init_polymorphic_object<Tlhs>(lhs, key_lhs, init_lhs);
      CStringElseCipher<is_rhs_cstr> rhs;
      init_polymorphic_object<Trhs>(rhs, key_rhs, init_rhs);

      Cipher resulting_cipher = append_else_concat<is_append>(lhs, rhs);

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

  }  // namespace concatenation

  namespace logic {
    void call_tests(TestCaseResource &);

    template <bool eq_op, typename Tlhs, typename Trhs>
    void check(const char *init_lhs, const int key_lhs, const char *init_rhs, const int key_rhs) {
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

  }  // namespace logic

  namespace iterator {
    void call_tests(TestCaseResource &);

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
    void check_offsetting(F iterator_operator, TAlgorithmParam key, const char *cstr, const IteratorOffsettingValues &iter_offsets) {
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

    const int LOGIC_FLAG = 0b1 << 2;
    const int EQ_FLAG = 0b1 << 1;
    const int GT_FLAG = 0b1;
    enum class OpType {
      lt = 0,
      gt = GT_FLAG,
      lte = EQ_FLAG,
      gte = EQ_FLAG | GT_FLAG,
      neq = LOGIC_FLAG,
      eq = LOGIC_FLAG | EQ_FLAG
    };
    template <typename ContainerType, typename FlagType>
    constexpr bool check_flag(ContainerType container, FlagType flag) {
      return static_cast<FlagType>(container) & flag;
    }

    template <OpType operation, typename TAlgorithm, typename TAlgorithmParam>
    return_if<check_flag(operation, LOGIC_FLAG), void>
    check_logic(TAlgorithmParam key, const char *cstr, size_t lhs_index, size_t rhs_index) {
      static_assert(!check_flag(operation, GT_FLAG), "GT flag will have no effect, should not even be set!");
      const bool is_equality = check_flag(operation, EQ_FLAG);

      Cipher cipher = allocate_initialized_cipher<TAlgorithm>(key, cstr);
      Cipher::const_iterator lhs_it = cipher.begin() + lhs_index;
      Cipher::const_iterator rhs_it = cipher.begin() + rhs_index;

      bool are_indecies_equal = lhs_index == rhs_index;
      bool result = is_equality ? lhs_it == rhs_it : lhs_it != rhs_it;
      bool expected_result = is_equality ? are_indecies_equal : !are_indecies_equal;

      EXPECT_EQ(result, expected_result)
        << "Logical comparison operator failed to return expected value for: (lhs iterator) " << (is_equality ? "==" : "!=") << " (rhs iterator)!";
    }

    template <OpType operation, typename TAlgorithm, typename TAlgorithmParam>
    return_if<!check_flag(operation, LOGIC_FLAG), void>
    check_logic(TAlgorithmParam key, const char *cstr, size_t small_index, size_t large_index) {
      const bool is_eq_allowed = static_cast<int>(operation) & EQ_FLAG;
      const bool is_gt = static_cast<int>(operation) & GT_FLAG;

      Cipher cipher = allocate_initialized_cipher<TAlgorithm>(key, cstr);
      auto evaluate_op = [cipher](size_t lhs_index, size_t rhs_index) -> bool {
        Cipher::const_iterator lhs_it = cipher.begin() + lhs_index;
        Cipher::const_iterator rhs_it = cipher.begin() + rhs_index;
        switch (operation) {
          case OpType::lt:
            return lhs_it < rhs_it;
          case OpType::gt:
            return lhs_it > rhs_it;
          case OpType::lte:
            return lhs_it <= rhs_it;
          case OpType::gte:
            return lhs_it >= rhs_it;
          default:
            throw std::range_error("The specified type of operation should not exist!");
        }
      };

      if (is_eq_allowed)
        EXPECT_TRUE(evaluate_op(small_index, small_index))
          << "Logical comparison operator which should allow equality, did not allow it!";
      EXPECT_EQ(evaluate_op(small_index, large_index), !is_gt)
        << "Logical comparison operator failed to return expected value for: (smaller index) [operator] (larger index)!";
      EXPECT_EQ(evaluate_op(large_index, small_index), is_gt)
        << "Logical comparison operator failed to return expected value for: (larger index) [operator] (smaller index)!";
    }

  }  // namespace iterator

}  // namespace testing
