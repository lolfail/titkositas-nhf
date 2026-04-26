#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <cstring>

#include "gtest_lite.h"
#include "TestHelpers.hpp"

#include "Cipher.h"
#include "CipherAlgorithm.h"

#include "CaesarCipher.h"
#include "XORCipher.h"

void call_indexing_tests();
void call_logical_operations_tests();
void call_concatenation_tests();
void call_iterator_tests();

inline const char* stringify(bool b) {
  return b ? "true" : "false";
}

int main() {
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

    Cipher to_copy = allocate_initialized_cipher<XORCipher>(key, cstr);

    Cipher copy_constructed = to_copy;
    EXPECT_STREQ(copy_constructed.c_str(), to_copy.c_str())
      << "Constructed object's stored text differs from source!";
  END

  TEST(assignation, cstr)
    const int key = 42;
    const char *cstr = "Hello World!";

    Cipher destination = allocate_initialized_cipher<XORCipher>(key, cstr);

    EXPECT_STREQ(destination.c_str(), cstr)
      << "Assigned object's stored text differs!";
  END
  TEST(assignation, class_same_key)
    const int key = 42;
    const char *cstr = "Hello World!";

    Cipher source = allocate_initialized_cipher<XORCipher>(key, cstr);

    Cipher destination(new XORCipher(key));

    destination = source;
    EXPECT_STREQ(destination.c_str(), source.c_str())
      << "Assigned objects' stored text differs!";
  END
  TEST(assignation, class_diff_key)
    const int key_source = 42;
    const int key_destination = 30;
    const char *cstr = "Hello World!";

    Cipher source = allocate_initialized_cipher<XORCipher>(key_source, cstr);

    Cipher destination(new XORCipher(key_destination));

    destination = source;
    EXPECT_STREQ(destination.c_str(), source.c_str())
      << "Assigned objects' stored text differs!";
  END

  call_indexing_tests();
  call_logical_operations_tests();
  call_concatenation_tests();
  call_iterator_tests();

  return 0;
}

void call_indexing_tests() {
  TEST(indexing, valid_index)
    const int key = 42;
    const char *cstr = "0123456789";
    const size_t index = 2;

    Cipher haystack = allocate_initialized_cipher<XORCipher>(key, cstr);

    EXPECT_NO_THROW(haystack[index]);
    EXPECT_EQ(haystack[index], cstr[index]);
  END
  TEST(indexing, out_of_bounds_index)
    const int key = 42;
    const char *cstr = "0123456789";
    const size_t index = strlen(cstr);

    Cipher haystack = allocate_initialized_cipher<XORCipher>(key, cstr);

    EXPECT_THROW(haystack[index], const std::out_of_range&);
  END
}

void call_logical_operations_tests() {
  TEST(logical_operations, equality_cstr)
    test_logic_operation<true, XORCipher, const char*>("abcdefg", 42, "abcdefg", 0);
  END
  TEST(logical_operations, inequality_cstr)
    test_logic_operation<false, XORCipher, const char*>("abcdefg", 42, "gfedcba", 0);
  END
  // TEST(logical_operations, equality_cstr_global)
  //   test_logic_operation<true, const char*, XORCipher>("abcdefg", 0, "abcdefg", 42);
  // END
  // assume all other operations can be false
  TEST(logical_operations, equality_cstr_false)
    test_logic_operation<true, XORCipher, const char*>("abc", 42, "def", 0);
  END
  TEST(logical_operations, inequality_cstr_false)
    test_logic_operation<false, XORCipher, const char*>("abc", 42, "abc", 0);
  END
  TEST(logical_operations, equality_same_key)
    test_logic_operation<true, XORCipher, XORCipher>("abcdefg", 42, "abcdefg", 42);
  END
  TEST(logical_operations, inequality_same_key)
    test_logic_operation<false, XORCipher, XORCipher>("abcdefg", 42, "gfedcba", 42);
  END
  TEST(logical_operations, equality_diff_key)
    test_logic_operation<true, XORCipher, XORCipher>("abcdefg", 42, "abcdefg", 30);
  END
  TEST(logical_operations, inequality_diff_key)
    test_logic_operation<false, XORCipher, XORCipher>("abcdefg", 42, "gfedcba", 30);
  END
  TEST(logical_operations, equality_diff_class)
    test_logic_operation<true, XORCipher, CaesarCipher>("abcdefg", 42, "abcdefg", -3);
  END
  TEST(logical_operations, inequality_diff_key)
    test_logic_operation<false, XORCipher, CaesarCipher>("abcdefg", 42, "gfedcba", -3);
  END
}

void call_concatenation_tests() {
  TEST(concatenation, append_cstr)
    test_concatenation<true, XORCipher, const char*>("Hello ", 42, "World!", 0);
  END
  TEST(concatenation, concat_cstr)
    test_concatenation<false, XORCipher, const char*>("Hello ", 42, "World!", 0);
  END
  // TEST(concatenation, concat_cstr_global)
  //   test_concatenation<false, const char*, XORCipher>("Hello ", 0, "World!", 42);
  // END
  TEST(concatenation, append_class_same_key)
    test_concatenation<true, XORCipher, XORCipher>("Hello ", 42, "World!", 42);
  END
  TEST(concatenation, concat_class_same_key)
    test_concatenation<false, XORCipher, XORCipher>("Hello ", 42, "World!", 42);
  END
  TEST(concatenation, append_class_diff_key)
    test_concatenation<true, XORCipher, XORCipher>("Hello ", 42, "World!", 30);
  END
  TEST(concatenation, concat_class_diff_key)
    test_concatenation<false, XORCipher, XORCipher>("Hello ", 42, "World!", 30);
  END

  // diff key for diff class is not needed as it is reciphered by default
  TEST(concatenation, append_diff_class)
    test_concatenation<true, XORCipher, CaesarCipher>("Hello ", 42, "World!", -3);
  END
  TEST(concatenation, concat_diff_class)
    test_concatenation<true, XORCipher, CaesarCipher>("Hello ", 42, "World!", -3);
  END
}

void call_iterator_tests() {
  TEST(iterator, begin_end_no_throw)
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, "Hello World!");

    EXPECT_NO_THROW(cipher.begin())
      << ".begin() failed!";
    EXPECT_NO_THROW(cipher.end())
      << ".end() failed!";
  END

  TEST(iterator, dereference)
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);
    Cipher::const_iterator it = cipher.begin();

    EXPECT_NO_THROW(*it)
      << "Dereferencing operator failed!";
    EXPECT_EQ(*it, cstr[0])
      << "Dereferencing differed from expected value!";
  END

  TEST(iterator, prefix)
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);
    Cipher::const_iterator it = cipher.begin();

    auto it_prefix_copy = ++it;
    EXPECT_EQ(*it_prefix_copy, *it)
      << "Prefix increment operator's pointed value differed from expected!";

    it_prefix_copy = --it;
    EXPECT_EQ(*it_prefix_copy, *it)
      << "Prefix decrement operator's pointed value differed from expected!";
  END
  TEST(iterator, postfix)
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);
    Cipher::const_iterator it = cipher.begin();

    auto it_postfix_copy = it++;
    EXPECT_EQ(*it, cstr[1])
      << "Iterator's pointed value differed from that of expected!";
    EXPECT_EQ(*it_postfix_copy, cstr[0])
      << "Postfix incremented iterator did not return a copy to the past!";

    it_postfix_copy = it--;
    EXPECT_EQ(*it, cstr[0])
      << "Iterator's pointed value differed from that of expected!";
    EXPECT_EQ(*it_postfix_copy, cstr[1])
      << "Postfix decremented iterator did not return a copy to the past!";
  END

  TEST(iterator, addition)
    const char *cstr = "abc";
    const unsigned int shift = 1;
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);

    Cipher::const_iterator it = cipher.begin();
    Cipher::const_iterator shifted_it = it + shift;

    EXPECT_EQ(*shifted_it, cstr[shift])
      << "Shifting iterator(positively) did not shift as expected!";
    EXPECT_EQ(*it, cstr[0])
      << "Original iterator was modified!";
  END
  TEST(iterator, substraction)
    const char *cstr = "abc";
    const unsigned int shift = 1;
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);

    Cipher::const_iterator it = cipher.begin() + shift;
    Cipher::const_iterator shifted_it = it - shift;

    EXPECT_EQ(*shifted_it, cstr[0])
      << "Shifting iterator(negatively) did not shift as expected!";
    EXPECT_EQ(*it, cstr[shift])
      << "Original iterator was modified!";
  END
  // TODO! test negative shifts

  TEST(iterator, distance)
    const int shift = 2;
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);

    Cipher::const_iterator it_lhs = cipher.begin() + shift;
    Cipher::const_iterator it_rhs = cipher.begin();

    int diff = it_lhs - it_rhs;
    EXPECT_EQ(diff, shift)
      << "The distance of the iterators does not match amount shifted from the beginning!";
  END

  TEST(iterator, indexing)
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);
    
    int shift = 1;
    int idx = 1;

    Cipher::const_iterator it = cipher.begin() + shift;

    EXPECT_EQ(it[idx], cstr[shift + idx])
      << "Indexing operator (positive integer) did not return the expected value!";

    shift = -1;
    idx = 1;
    it = cipher.begin() + shift;

    EXPECT_EQ(it[idx], cstr[shift + idx])
      << "Indexing operator (negative integer) did not return the expected value!";
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
