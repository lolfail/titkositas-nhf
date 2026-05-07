#include <cstddef>
#include <stdexcept>
#include <cstring>

#include "memtrace.h"
#include "gtest_lite.h"
#include "GenericTests.hpp"

#include "Cipher.h"

#include "CaesarCipher.h"
#include "XORCipher.h"

/**
 * Megjegyzés:
 * A tesztesetek jelenleg statikusak, olyan értelemben, hogy még nem fájlból olvasódnak be a tesztadatok,
 * hanem a tesztekbe vannak "égetve". Azonban ez csak egyelőre van így.
 *
 * Verziókövetés:
 * https://github.com/lolfail/titkositas-nhf
 */

int main() {
  using namespace testing;

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

    EXPECT_THROW(haystack[index], const std::out_of_range);
  END

  assignation::call_tests();
  concatenation::call_tests();
  logic::call_tests();
  iterator::call_tests();

  return 0;
}

void testing::assignation::call_tests() {
  using namespace assignation;

  TEST(assignation, empty_c_str_call)
    const int key = 42;
    Cipher empty(new XORCipher(key));

    EXPECT_NO_THROW(empty.c_str())
      << "Did not handle empty string without errors!";
    EXPECT_STREQ(empty.c_str(), "")
      << "Queried cstring is not an empty string (or not NUL terminated)!";
  END
  TEST(assignation, copy_constructor)
    const int key = 42;
    const char *cstr = "Hello World!";

    Cipher to_copy = allocate_initialized_cipher<XORCipher>(key, cstr);

    Cipher copy_constructed = to_copy;
    EXPECT_STREQ(copy_constructed.c_str(), to_copy.c_str())
      << "Constructed object's stored text differs from source!";
  END

  TEST(assignation, assign_cstr)
    check<XORCipher, const char*>(42, 0, "Hello World!");
  END

  TEST(assignation, assign_class)
    // same algo same key
    check<XORCipher, XORCipher>(42, 42, "Hello World!");
    // same algo diff key
    check<XORCipher, XORCipher>(42, 32, "Hello World!");
    // diff algo
    check<XORCipher, CaesarCipher>(42, -3, "Hello World!");
  END
}

void testing::logic::call_tests() {
  using namespace logic;

  TEST(logical_operations, equality_cstr)
    // check for true
    check<true, XORCipher, const char*>("abcdefg", 42, "abcdefg", 0);
    // check for false
    check<true, XORCipher, const char*>("abc", 42, "def", 0);
    // check global overload
    check<true, const char*, XORCipher>("abcdefg", 0, "abcdefg", 42);
  END
  TEST(logical_operations, inequality_cstr)
    // check for true
    check<false, XORCipher, const char*>("abcdefg", 42, "gfedcba", 0);
    // check for false
    check<false, XORCipher, const char*>("abc", 42, "abc", 0);
    // check global overload
    check<false, const char*, XORCipher>("abcdefg", 0, "abcdefg", 42);
  END

  TEST(logical_operations, equality_class)
    // same algo same key
    check<true, XORCipher, XORCipher>("abcdefg", 42, "abcdefg", 42);
    // same algo diff key
    check<true, XORCipher, XORCipher>("abcdefg", 42, "abcdefg", 30);
    // diff algo
    check<true, XORCipher, CaesarCipher>("abcdefg", 42, "abcdefg", -3);
  END
  TEST(logical_operations, inequality_class)
    // same algo same key
    check<false, XORCipher, XORCipher>("abcdefg", 42, "gfedcba", 42);
    // same algo diff key
    check<false, XORCipher, XORCipher>("abcdefg", 42, "gfedcba", 30);
    // diff algo
    check<false, XORCipher, CaesarCipher>("abcdefg", 42, "gfedcba", -3);
  END
}

void testing::concatenation::call_tests() {
  using namespace concatenation;

  TEST(concatenation, append_cstr)
    check<true, XORCipher, const char*>("Hello ", 42, "World!", 0);
  END
  TEST(concatenation, concat_cstr)
    check<false, XORCipher, const char*>("Hello ", 42, "World!", 0);
    check<false, const char*, XORCipher>("Hello ", 0, "World!", 42);
  END
  TEST(concatenation, append_class)
    // same algo same key
    check<true, XORCipher, XORCipher>("Hello ", 42, "World!", 42);
    // same algo diff key
    check<true, XORCipher, XORCipher>("Hello ", 42, "World!", 30);
    // diff algo
    check<true, XORCipher, CaesarCipher>("Hello ", 42, "World!", -3);

  END
  TEST(concatenation, concat_class)
    // same algo same key
    check<false, XORCipher, XORCipher>("Hello ", 42, "World!", 42);
    // same algo diff key
    check<false, XORCipher, XORCipher>("Hello ", 42, "World!", 30);
    // diff algo
    check<true, XORCipher, CaesarCipher>("Hello ", 42, "World!", -3);
  END
}

void testing::iterator::call_tests() {
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

  TEST(iterator, add_sub_distance)
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, "abcdefg");
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
    EXPECT_NE(&it_shifted, &it) // TODO! smarter check
      << "Subtraction on an iterator modified original iterator!";
  END

  TEST(iterator, prefix)
    auto prefix_decrement_callback = [](Cipher::const_iterator &it) { return --it; };
    check_offsetting<XORCipher>(prefix_decrement_callback, 42, "Hello World!", IteratorOffsettingValues(2, 1, 1));

    auto prefix_increment_callback = [](Cipher::const_iterator &it) { return ++it; };
    check_offsetting<XORCipher>(prefix_increment_callback, 42, "Hello World!", IteratorOffsettingValues(2, 3, 3));
  END
  TEST(iterator, postfix)
    auto prefix_decrement_callback = [](Cipher::const_iterator &it) { return it--; };
    check_offsetting<XORCipher>(prefix_decrement_callback, 42, "Hello World!", IteratorOffsettingValues(2, 2, 1));

    auto prefix_increment_callback = [](Cipher::const_iterator &it) { return it++; };
    check_offsetting<XORCipher>(prefix_increment_callback, 42, "Hello World!", IteratorOffsettingValues(2, 2, 3));
  END

  TEST(iterator, compound_assignment)
    const size_t shift = 2;
    const char *cstr = "Hello World!";

    auto compound_add = [shift](Cipher::const_iterator &it) {return it += shift;};
    check_offsetting<XORCipher>(compound_add, 42, cstr, IteratorOffsettingValues(2, 4, 4));

    auto compound_sub = [shift](Cipher::const_iterator &it) {return it -= shift;};
    check_offsetting<XORCipher>(compound_sub, 42, cstr, IteratorOffsettingValues(2, 0, 0));
  END

  TEST(iterator, indexing)
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);

    auto index_test =[cipher, cstr](int shift, int idx) {
      Cipher::const_iterator it = cipher.begin() + shift;
      EXPECT_EQ(it[idx], cstr[shift + idx])
        << "Indexing operator did not yield expected result(" << (idx > 0 ? "positive" : "negative") << " index)!";
    };

    index_test(1, 1);
    index_test(1, -1);
  END

  TEST(iterator, logic_equality_inequality)
    const char *cstr = "abcdefga";
    const int key = 42;

    check_logic<OpType::eq, XORCipher>(key, cstr, 0, 0);
    check_logic<OpType::eq, XORCipher>(key, cstr, 0, 4);
    check_logic<OpType::eq, XORCipher>(key, cstr, 0, 3);

    check_logic<OpType::neq, XORCipher>(key, cstr, 0, 0);
    check_logic<OpType::neq, XORCipher>(key, cstr, 0, 4);
    check_logic<OpType::neq, XORCipher>(key, cstr, 0, 3);
  END

  TEST(iterator, logic_comparison)
    const char *cstr = "abcdefga";
    const int key = 42;

    check_logic<OpType::lt, XORCipher>(key, cstr, 0, 1);
    check_logic<OpType::lte, XORCipher>(key, cstr, 0, 1);
    check_logic<OpType::gt, XORCipher>(key, cstr, 0, 1);
    check_logic<OpType::gte, XORCipher>(key, cstr, 0, 1);
  END

  TEST(iterator, foreach)
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, "Hello World");

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
