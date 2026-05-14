#include <cstddef>
#include <stdexcept>
#include <cstring>

#include "memtrace.h"
#include "gtest_lite.h"
#include "GenericTests.hpp"

#include <fstream>
#include "TestFileHandling.hpp"

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
  std::ifstream stream;
  stream.open("default_tests.txt");
  TestCaseResource testcase_file(stream, ", ", "\n");

  using namespace testing;

  TEST(indexing, valid_index) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_ptr = testcase_file.read_next<char[]>();
    size_t index = *testcase_file.read_next<size_t>();

    Cipher haystack = allocate_initialized_cipher<XORCipher>(key, cstr_ptr.get());

    EXPECT_NO_THROW(haystack[index]);
    EXPECT_EQ(haystack[index], cstr_ptr.get()[index]);
  } END
  TEST(indexing, out_of_bounds_index) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_ptr = testcase_file.read_next<char[]>();

    Cipher haystack = allocate_initialized_cipher<XORCipher>(key, cstr_ptr.get());
    size_t len = strlen(cstr_ptr.get());

    EXPECT_THROW(haystack[len], const std::out_of_range);
  } END

  assignation::call_tests(testcase_file);
  concatenation::call_tests(testcase_file);
  logic::call_tests(testcase_file);
  iterator::call_tests(testcase_file);

  return 0;
}

void testing::assignation::call_tests(TestCaseResource &testcase_file) {
  using namespace assignation;

  TEST(assignation, empty_c_str_call) {
    int key = *testcase_file.read_next<int>();

    Cipher empty(new XORCipher(key));

    EXPECT_NO_THROW(empty.c_str())
      << "Did not handle empty string without errors!";
    EXPECT_STREQ(empty.c_str(), "")
      << "Queried cstring is not an empty string (or not NUL terminated)!";
  } END

  TEST(assignation, copy_constructor) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_ptr = testcase_file.read_next<char[]>();

    Cipher to_copy = allocate_initialized_cipher<XORCipher>(key, cstr_ptr.get());

    Cipher copy_constructed = to_copy;
    EXPECT_STREQ(copy_constructed.c_str(), to_copy.c_str())
      << "Constructed object's stored text differs from source!";
  } END

  TEST(assignation, assign_cstr) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_ptr = testcase_file.read_next<char[]>();

    check<XORCipher, const char*>(key, 0, cstr_ptr.get());
  } END

  TEST(assignation, assign_class) {
    int xor_key_a = *testcase_file.read_next<int>();
    int xor_key_b = *testcase_file.read_next<int>();
    int caesar_key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr = testcase_file.read_next<char[]>();

    // same algo same key
    check<XORCipher, XORCipher>(xor_key_a, xor_key_a, cstr.get());
    // same algo diff key
    check<XORCipher, XORCipher>(xor_key_a, xor_key_b, cstr.get());
    // diff algo
    check<XORCipher, CaesarCipher>(xor_key_a, caesar_key, cstr.get());
  } END
}

void testing::concatenation::call_tests(TestCaseResource &testcase_file) {
  using namespace concatenation;

  TEST(concatenation, append_cstr) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_base = testcase_file.read_next<char[]>();
    unique_ptr_lite<char[]> cstr_appendix = testcase_file.read_next<char[]>();

    check<true, XORCipher, const char*>(cstr_base.get(), key, cstr_appendix.get(), 0);
  } END
  TEST(concatenation, concat_cstr) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_base = testcase_file.read_next<char[]>();
    unique_ptr_lite<char[]> cstr_appendix = testcase_file.read_next<char[]>();

    check<false, XORCipher, const char*>(cstr_base.get(), key, cstr_appendix.get(), 0);
    check<false, const char*, XORCipher>(cstr_base.get(), 0, cstr_appendix.get(), key);
  } END
  TEST(concatenation, append_class) {
    int key_xor_a = *testcase_file.read_next<int>();
    int key_xor_b = *testcase_file.read_next<int>();
    int key_caesar = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_base = testcase_file.read_next<char[]>();
    unique_ptr_lite<char[]> cstr_appendix = testcase_file.read_next<char[]>();

    // same algo same key
    check<true, XORCipher, XORCipher>(cstr_base.get(), key_xor_a, cstr_appendix.get(), key_xor_a);
    // same algo diff key
    check<true, XORCipher, XORCipher>(cstr_base.get(), key_xor_a, cstr_appendix.get(), key_xor_b);
    // diff algo
    check<true, XORCipher, CaesarCipher>(cstr_base.get(), key_xor_a, cstr_appendix.get(), key_caesar);
  } END
  TEST(concatenation, concat_class) {
    int key_xor_a = *testcase_file.read_next<int>();
    int key_xor_b = *testcase_file.read_next<int>();
    int key_caesar = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_base = testcase_file.read_next<char[]>();
    unique_ptr_lite<char[]> cstr_appendix = testcase_file.read_next<char[]>();

    // same algo same key
    check<false, XORCipher, XORCipher>(cstr_base.get(), key_xor_a, cstr_appendix.get(), key_xor_a);
    // same algo diff key
    check<false, XORCipher, XORCipher>(cstr_base.get(), key_xor_a, cstr_appendix.get(), key_xor_b);
    // diff algo
    check<false, XORCipher, CaesarCipher>(cstr_base.get(), key_xor_a, cstr_appendix.get(), key_caesar);
  } END
}

void testing::logic::call_tests(TestCaseResource &testcase_file) {
  using namespace logic;

  TEST(logical_operations, equality_cstr) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_base = testcase_file.read_next<char[]>();
    unique_ptr_lite<char[]> cstr_diff = testcase_file.read_next<char[]>();

    // check for true
    check<true, XORCipher, const char*>(cstr_base.get(), key, cstr_base.get(), 0);
    // check for false
    check<true, XORCipher, const char*>(cstr_base.get(), key, cstr_diff.get(), 0);
    // check global overload
    check<true, const char*, XORCipher>(cstr_base.get(), 0, cstr_base.get(), key);
  } END
  TEST(logical_operations, inequality_cstr) {
    int key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_base = testcase_file.read_next<char[]>();
    unique_ptr_lite<char[]> cstr_diff = testcase_file.read_next<char[]>();

    // check for true
    check<false, XORCipher, const char*>(cstr_base.get(), key, cstr_base.get(), 0);
    // check for false
    check<false, XORCipher, const char*>(cstr_base.get(), key, cstr_diff.get(), 0);
    // check global overload
    check<false, const char*, XORCipher>(cstr_base.get(), 0, cstr_base.get(), key);
  } END

  TEST(logical_operations, equality_class) {
    int xor_key_a = *testcase_file.read_next<int>();
    int xor_key_b = *testcase_file.read_next<int>();
    int caesar_key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr = testcase_file.read_next<char[]>(); // infinite loop on get part size while

    // same algo same key
    check<true, XORCipher, XORCipher>(cstr.get(), xor_key_a, cstr.get(), xor_key_a);
    // same algo diff key
    check<true, XORCipher, XORCipher>(cstr.get(), xor_key_a, cstr.get(), xor_key_b);
    // diff algo
    check<true, XORCipher, CaesarCipher>(cstr.get(), xor_key_a, cstr.get(), caesar_key);
  } END
  TEST(logical_operations, inequality_class) {
    int xor_key_a = *testcase_file.read_next<int>();
    int xor_key_b = *testcase_file.read_next<int>();
    int caesar_key = *testcase_file.read_next<int>();
    unique_ptr_lite<char[]> cstr_base = testcase_file.read_next<char[]>();
    unique_ptr_lite<char[]> cstr_diff = testcase_file.read_next<char[]>();

    // same algo same key
    check<false, XORCipher, XORCipher>(cstr_base.get(), xor_key_a, cstr_diff.get(), xor_key_a);
    // same algo diff key
    check<false, XORCipher, XORCipher>(cstr_base.get(), xor_key_a, cstr_diff.get(), xor_key_b);
    // diff algo
    check<false, XORCipher, CaesarCipher>(cstr_base.get(), xor_key_a, cstr_diff.get(), caesar_key);
  } END
}

void testing::iterator::call_tests(TestCaseResource &testcase_file) {
  TEST(iterator, begin_end_no_throw) {
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, "Hello World!");

    EXPECT_NO_THROW(cipher.begin())
      << ".begin() failed!";
    EXPECT_NO_THROW(cipher.end())
      << ".end() failed!";
  } END

  TEST(iterator, dereference) {
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);
    Cipher::const_iterator it = cipher.begin();

    EXPECT_NO_THROW(*it)
      << "Dereferencing operator failed!";
    EXPECT_EQ(*it, cstr[0])
      << "Dereferencing differed from expected value!";
  } END

  TEST(iterator, add_sub_distance) {
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
  } END

  TEST(iterator, prefix) {
    auto prefix_decrement_callback = [](Cipher::const_iterator &it) { return --it; };
    check_offsetting<XORCipher>(prefix_decrement_callback, 42, "Hello World!", IteratorOffsettingValues(2, 1, 1));

    auto prefix_increment_callback = [](Cipher::const_iterator &it) { return ++it; };
    check_offsetting<XORCipher>(prefix_increment_callback, 42, "Hello World!", IteratorOffsettingValues(2, 3, 3));
  } END
  TEST(iterator, postfix) {
    auto prefix_decrement_callback = [](Cipher::const_iterator &it) { return it--; };
    check_offsetting<XORCipher>(prefix_decrement_callback, 42, "Hello World!", IteratorOffsettingValues(2, 2, 1));

    auto prefix_increment_callback = [](Cipher::const_iterator &it) { return it++; };
    check_offsetting<XORCipher>(prefix_increment_callback, 42, "Hello World!", IteratorOffsettingValues(2, 2, 3));
  } END

  TEST(iterator, compound_assignment) {
    const size_t shift = 2;
    const char *cstr = "Hello World!";

    auto compound_add = [shift](Cipher::const_iterator &it) {return it += shift;};
    check_offsetting<XORCipher>(compound_add, 42, cstr, IteratorOffsettingValues(2, 4, 4));

    auto compound_sub = [shift](Cipher::const_iterator &it) {return it -= shift;};
    check_offsetting<XORCipher>(compound_sub, 42, cstr, IteratorOffsettingValues(2, 0, 0));
  } END

  TEST(iterator, indexing) {
    const char *cstr = "abc";
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, cstr);

    auto index_test =[cipher, cstr](int shift, int idx) {
      Cipher::const_iterator it = cipher.begin() + shift;
      EXPECT_EQ(it[idx], cstr[shift + idx])
        << "Indexing operator did not yield expected result(" << (idx > 0 ? "positive" : "negative") << " index)!";
    };

    index_test(1, 1);
    index_test(1, -1);
  } END

  TEST(iterator, logic_equality_inequality) {
    const char *cstr = "abcdefga";
    const int key = 42;

    check_logic<OpType::eq, XORCipher>(key, cstr, 0, 0);
    check_logic<OpType::eq, XORCipher>(key, cstr, 0, 4);
    check_logic<OpType::eq, XORCipher>(key, cstr, 0, 3);

    check_logic<OpType::neq, XORCipher>(key, cstr, 0, 0);
    check_logic<OpType::neq, XORCipher>(key, cstr, 0, 4);
    check_logic<OpType::neq, XORCipher>(key, cstr, 0, 3);
  } END

  TEST(iterator, logic_comparison) {
    const char *cstr = "abcdefga";
    const int key = 42;

    check_logic<OpType::lt, XORCipher>(key, cstr, 0, 1);
    check_logic<OpType::lte, XORCipher>(key, cstr, 0, 1);
    check_logic<OpType::gt, XORCipher>(key, cstr, 0, 1);
    check_logic<OpType::gte, XORCipher>(key, cstr, 0, 1);
  } END

  TEST(iterator, foreach) {
    Cipher cipher = allocate_initialized_cipher<XORCipher>(42, "Hello World");

    char *deciphered_buf = new char[cipher.len() + 1]; // defer delete

    size_t last_index = 0;
    for (auto c : cipher)
      deciphered_buf[last_index++] = c;
    deciphered_buf[last_index] = '\0';

    EXPECT_STREQ(deciphered_buf, cipher.c_str())
      << "c_str() and foreach deciphered strings differ!";

    delete[] deciphered_buf;
  } END
}
