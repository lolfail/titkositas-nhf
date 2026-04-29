#include <cstddef>
#include <stdexcept>
#include <cstring>

#include "memtrace.h"
#include "gtest_lite.h"
#include "GenericTests.hpp"

#include "Cipher.h"
#include "CipherAlgorithm.h"

#include "CaesarCipher.h"
#include "XORCipher.h"

int main() {
  using namespace testing;

  TEST(integrity, empty_c_str_call)
    const int key = 42;
    Cipher empty(new XORCipher(key));

    EXPECT_NO_THROW(empty.c_str())
      << "Did not handle empty string without errors!";
    EXPECT_STREQ(empty.c_str(), "")
      << "Queried cstring is not an empty string (or not NUL terminated)!";
  END
  TEST(integrity, copy_constructor)
    const int key = 42;
    const char *cstr = "Hello World!";

    Cipher to_copy = testing::allocate_initialized_cipher<XORCipher>(key, cstr);

    Cipher copy_constructed = to_copy;
    EXPECT_STREQ(copy_constructed.c_str(), to_copy.c_str())
      << "Constructed object's stored text differs from source!";
  END

  TEST(assignation, cstr)
    const int key = 42;
    const char *cstr = "Hello World!";

    Cipher destination = testing::allocate_initialized_cipher<XORCipher>(key, cstr);

    EXPECT_STREQ(destination.c_str(), cstr)
      << "Assigned object's stored text differs!";
  END
  TEST(assignation, class_same_key)
    const int key = 42;
    const char *cstr = "Hello World!";

    Cipher source = testing::allocate_initialized_cipher<XORCipher>(key, cstr);

    Cipher destination(new XORCipher(key));

    destination = source;
    EXPECT_STREQ(destination.c_str(), source.c_str())
      << "Assigned objects' stored text differs!";
  END
  TEST(assignation, class_diff_key)
    const int key_source = 42;
    const int key_destination = 30;
    const char *cstr = "Hello World!";

    Cipher source = testing::allocate_initialized_cipher<XORCipher>(key_source, cstr);

    Cipher destination(new XORCipher(key_destination));

    destination = source;
    EXPECT_STREQ(destination.c_str(), source.c_str())
      << "Assigned objects' stored text differs!";
  END

  TEST(indexing, valid_index)
    const int key = 42;
    const char *cstr = "0123456789";
    const size_t index = 2;

    Cipher haystack = testing::allocate_initialized_cipher<XORCipher>(key, cstr);

    EXPECT_NO_THROW(haystack[index]);
    EXPECT_EQ(haystack[index], cstr[index]);
  END
  TEST(indexing, out_of_bounds_index)
    const int key = 42;
    const char *cstr = "0123456789";
    const size_t index = strlen(cstr);

    Cipher haystack = testing::allocate_initialized_cipher<XORCipher>(key, cstr);

    EXPECT_THROW(haystack[index], const std::out_of_range&);
  END

  concatenation::call_tests();
  logic::call_tests();
  iterator::call_tests();

  return 0;
}

void testing::logic::call_tests() {
  TEST(logical_operations, equality_cstr)
    check<true, XORCipher, const char*>("abcdefg", 42, "abcdefg", 0);
  END
  TEST(logical_operations, inequality_cstr)
    check<false, XORCipher, const char*>("abcdefg", 42, "gfedcba", 0);
  END
  // TEST(logical_operations, equality_cstr_global)
  //   test_logic_operation<true, const char*, XORCipher>("abcdefg", 0, "abcdefg", 42);
  // END
  // assume all other operations can be false
  TEST(logical_operations, equality_cstr_false)
    check<true, XORCipher, const char*>("abc", 42, "def", 0);
  END
  TEST(logical_operations, inequality_cstr_false)
    check<false, XORCipher, const char*>("abc", 42, "abc", 0);
  END
  TEST(logical_operations, equality_same_key)
    check<true, XORCipher, XORCipher>("abcdefg", 42, "abcdefg", 42);
  END
  TEST(logical_operations, inequality_same_key)
    check<false, XORCipher, XORCipher>("abcdefg", 42, "gfedcba", 42);
  END
  TEST(logical_operations, equality_diff_key)
    check<true, XORCipher, XORCipher>("abcdefg", 42, "abcdefg", 30);
  END
  TEST(logical_operations, inequality_diff_key)
    check<false, XORCipher, XORCipher>("abcdefg", 42, "gfedcba", 30);
  END
  TEST(logical_operations, equality_diff_class)
    check<true, XORCipher, CaesarCipher>("abcdefg", 42, "abcdefg", -3);
  END
  TEST(logical_operations, inequality_diff_key)
    check<false, XORCipher, CaesarCipher>("abcdefg", 42, "gfedcba", -3);
  END
}

void testing::concatenation::call_tests() {
  TEST(concatenation, append_cstr)
    check<true, XORCipher, const char*>("Hello ", 42, "World!", 0);
  END
  TEST(concatenation, concat_cstr)
    check<false, XORCipher, const char*>("Hello ", 42, "World!", 0);
  END
  // TEST(concatenation, concat_cstr_global)
  //   test_concatenation<false, const char*, XORCipher>("Hello ", 0, "World!", 42);
  // END
  TEST(concatenation, append_class_same_key)
    check<true, XORCipher, XORCipher>("Hello ", 42, "World!", 42);
  END
  TEST(concatenation, concat_class_same_key)
    check<false, XORCipher, XORCipher>("Hello ", 42, "World!", 42);
  END
  TEST(concatenation, append_class_diff_key)
    check<true, XORCipher, XORCipher>("Hello ", 42, "World!", 30);
  END
  TEST(concatenation, concat_class_diff_key)
    check<false, XORCipher, XORCipher>("Hello ", 42, "World!", 30);
  END

  // diff key for diff class is not needed as it is reciphered by default
  TEST(concatenation, append_diff_class)
    check<true, XORCipher, CaesarCipher>("Hello ", 42, "World!", -3);
  END
  TEST(concatenation, concat_diff_class)
    check<true, XORCipher, CaesarCipher>("Hello ", 42, "World!", -3);
  END
}

void testing::iterator::call_tests() {
  TEST(iterator, begin_end_no_throw)
    Cipher cipher = testing::allocate_initialized_cipher<XORCipher>(42, "Hello World!");

    EXPECT_NO_THROW(cipher.begin())
      << ".begin() failed!";
    EXPECT_NO_THROW(cipher.end())
      << ".end() failed!";
  END

  TEST(iterator, dereference)
    const char *cstr = "abc";
    Cipher cipher = testing::allocate_initialized_cipher<XORCipher>(42, cstr);
    Cipher::const_iterator it = cipher.begin();

    EXPECT_NO_THROW(*it)
      << "Dereferencing operator failed!";
    EXPECT_EQ(*it, cstr[0])
      << "Dereferencing differed from expected value!";
  END

  TEST(iterator, add_sub_distance)
    const char *cstr = "abcdefg";
    Cipher cipher = testing::allocate_initialized_cipher<XORCipher>(42, cstr);
    Cipher::const_iterator it = cipher.begin();

    const size_t shift = 2;
    Cipher::const_iterator it_shifted = it + shift;
    EXPECT_EQ(iterator_position_from_start(cipher, it_shifted), ptrdiff_t(shift))
      << "Addition on an iterator did not work correctly or distance was reported falsely!";
    EXPECT_NE(iterator_position_from_start(cipher, it_shifted), iterator_position_from_start(cipher, it))
      << "Addition on an iterator modified original iterator!";

    it_shifted = (it + shift) - shift;
    EXPECT_EQ(iterator_position_from_start(cipher, it_shifted), 0)
      << "Addition then subtraction of the same value on the same iterator failed to return it to the beginning!";
    EXPECT_NE(&it_shifted, &it) // meh
      << "Subtraction on an iterator modified original iterator!";
  END

  TEST(iterator, prefix_dec)
    auto prefix_decrement_callback = [](Cipher::const_iterator &it) { return --it; };
    testing::iterator::check_offsetting<XORCipher>(prefix_decrement_callback, 42, "Hello World!", IteratorOffsettingValues(2, 1, 1));
  END
  TEST(iterator, prefix_inc)
    auto prefix_increment_callback = [](Cipher::const_iterator &it) { return ++it; };
    testing::iterator::check_offsetting<XORCipher>(prefix_increment_callback, 42, "Hello World!", IteratorOffsettingValues(2, 3, 3));
  END
  TEST(iterator, postfix_dec)
    auto prefix_decrement_callback = [](Cipher::const_iterator &it) { return it--; };
    testing::iterator::check_offsetting<XORCipher>(prefix_decrement_callback, 42, "Hello World!", IteratorOffsettingValues(2, 2, 1));
  END
  TEST(iterator, postfix_inc)
    auto prefix_increment_callback = [](Cipher::const_iterator &it) { return it++; };
    testing::iterator::check_offsetting<XORCipher>(prefix_increment_callback, 42, "Hello World!", IteratorOffsettingValues(2, 2, 3));
  END

  TEST(iterator, compound_assignment_add)
    const size_t shift = 2;
    const char *cstr = "Hello World!";

    auto compound_add = [shift](Cipher::const_iterator &it) {return it += shift;};
    testing::iterator::check_offsetting<XORCipher>(compound_add, 42, cstr, IteratorOffsettingValues(2, 4, 4));
  END
  TEST(iterator, compound_assigment_sub)
    const size_t shift = 2;
    const char *cstr = "Hello World!";

    auto compound_sub = [shift](Cipher::const_iterator &it) {return it -= shift;};
    testing::iterator::check_offsetting<XORCipher>(compound_sub, 42, cstr, IteratorOffsettingValues(2, 0, 0));
  END

  TEST(iterator, indexing)
    const char *cstr = "abc";
    Cipher cipher = testing::allocate_initialized_cipher<XORCipher>(42, cstr);

    auto index_test =[cipher, cstr](int shift, int idx) {
      Cipher::const_iterator it = cipher.begin() + shift;
      EXPECT_EQ(it[idx], cstr[shift + idx])
        << "Indexing operator did not yield expected result(" << (idx > 0 ? "positive" : "negative") << " index)!";
    };

    index_test(1, 1);
    index_test(1, -1);
  END

  // there isn't any dynamic test naming functionality :(
  const char *cstr = "abcdefga";
  TEST(iterator, logic_eq)
    check_logic<OpType::eq, XORCipher>(42, cstr, 0, 0);
    check_logic<OpType::eq, XORCipher>(42, cstr, 0, 4);
    check_logic<OpType::eq, XORCipher>(42, cstr, 0, 3);
  END
  TEST(iterator, logic_neq)
    check_logic<OpType::neq, XORCipher>(42, cstr, 0, 0);
    check_logic<OpType::neq, XORCipher>(42, cstr, 0, 4);
    check_logic<OpType::neq, XORCipher>(42, cstr, 0, 3);
  END

  TEST(iterator, logic_lt)
    check_logic<OpType::lt, XORCipher>(42, cstr, 0, 1);
  END
  TEST(iterator, logic_lte)
    check_logic<OpType::lte, XORCipher>(42, cstr, 0, 1);
  END
  TEST(iterator, logic_gt)
    check_logic<OpType::gt, XORCipher>(42, cstr, 0, 1);
  END
  TEST(iterator, logic_gte)
    check_logic<OpType::gte, XORCipher>(42, cstr, 0, 1);
  END

  TEST(iterator, foreach)
    const int key = 42;
    const char *cstr = "Hello World!";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(key, cstr);

    char *deciphered_buf = new char[cipher.len() + 1]; // defer delete

    size_t last_index = 0;
    for (auto c : cipher)
      deciphered_buf[last_index++] = c;
    deciphered_buf[last_index] = '\0';

    EXPECT_STREQ(deciphered_buf, cipher.c_str())
      << "c_str() and foreach deciphered strings differ!";

    delete[] deciphered_buf;
  END
}
