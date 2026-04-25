#include <iostream>
#include <stdexcept>
#include <cstring>

#include "gtest_lite.h"
#include "TestHelpers.hpp"

#include "Cipher.h"
#include "CipherAlgorithm.h"

#include "CaesarCipher.h"
#include "XORCipher.h"

void log_cipher(Cipher &cipher, const char *testcase = "") {
  std::cout << testcase << std::endl;
  std::cout << '\t';
  for (auto it : cipher)
    std::cout << it;
  std::cout << std::endl;
}
void log_bool(bool test_result, const char *testcase = "") {
  std::cout << testcase << std::endl;
  std::cout << '\t' << (test_result ? "OK, TRUE!" : "WRONG, FALSE!") << std::endl;
}

int main2();

#define CIPHERALGO XORCipher
int main() {
  return main2();
  Cipher og_cipher = Cipher(new CIPHERALGO(42));
  og_cipher = "Hello";
  log_cipher(og_cipher, "Did it get set?");

  Cipher deep_copied_og_cipher = og_cipher;
  deep_copied_og_cipher += " World!";
  log_cipher(deep_copied_og_cipher, "(+=) addition works (modifies)?");

  log_cipher(og_cipher, "Original is still untouched?");

  Cipher addition_cipher = og_cipher + " World++!";
  log_cipher(addition_cipher, "(+) addition worked?");
  log_cipher(og_cipher, "(+) addition worked - original untouched?");

  Cipher hello_cipher(new CIPHERALGO(30));
  Cipher world_cipher = hello_cipher;
  hello_cipher = "Hello";
  world_cipher = " World!";
  Cipher cumulative = hello_cipher + world_cipher;
  log_cipher(cumulative, "(+) addition of classes worked?");
  hello_cipher += world_cipher;
  log_cipher(hello_cipher, "(+=) addition of classes worked?");

  Cipher key2(new CIPHERALGO(2));
  key2 = "key2 ";
  Cipher key3(new CIPHERALGO(3));
  key3 = "key3";
  Cipher keyadded = key2 + key3;
  log_cipher(keyadded, "key 2 and key 3 were added resulting in:");

  // check whether recipher_and_assign_data keeps key??

  Cipher eq_og(new CIPHERALGO(42));
  eq_og = "same";
  Cipher eq_same_key = eq_og;
  log_bool(eq_og == eq_same_key, "were the two (same key) equal?");
  Cipher eq_diff_key(new CIPHERALGO(30));
  eq_diff_key.recipher_and_assign_data(eq_og);
  log_bool(eq_og == eq_diff_key, "were the two (different key) equal?");
  Cipher eq_diff = eq_og;
  eq_diff = "different";
  log_bool(eq_og != eq_diff, "were the two (same) different?");

  return 0;
}

/*
 * Tests:
 *   - functionality only in one (which essentially tests the algorithm itself and it's implementation) => is it even part of the project?
 *   - common functionality => larger case, includes things like concatenation and reciphering(!!!)
 *   - memory violation checks (automatic in a sense)
 */

int main2() {
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

  TEST(concatenation, append_cstr)
    test_concatenation<true, true, false>("Hello ", 42, "World!", 0);
  END
  TEST(concatenation, concat_cstr)
    test_concatenation<false, true, false>("Hello ", 42, "World!", 0);
  END
  // TEST(concatenation, concat_cstr_global)
  //   test_concatenation<false, false, true>("Hello ", 0, "World!", 42);
  // END
  TEST(concatenation, append_class_same_key)
    test_concatenation<true, true, true>("Hello ", 42, "World!", 42);
  END
  TEST(concatenation, concat_class_same_key)
    test_concatenation<false, true, true>("Hello ", 42, "World!", 42);
  END
  TEST(concatenation, append_class_diff_key)
    test_concatenation<true, true, true>("Hello ", 42, "World!", 30);
  END
  TEST(concatenation, concat_class_diff_key)
    test_concatenation<false, true, true>("Hello ", 42, "World", 30);
  END

  TEST(indexing, valid_index)
    const int key = 42;
    const char *cstr = "0123456789";
    const size_t index = 2;

    Cipher haystack = allocate_initialized_cipher<XORCipher>(42, cstr);

    EXPECT_NO_THROW(haystack[index]);
    EXPECT_EQ(haystack[index], cstr[index]);
  END
  TEST(indexing, out_of_bounds_index)
    const int key = 42;
    const char *cstr = "0123456789";
    const size_t index = strlen(cstr);

    Cipher haystack = allocate_initialized_cipher<XORCipher>(42, cstr);

    EXPECT_THROW(haystack[index], std::out_of_range);
  END

  // TEST(logical_operations, equality_diff_key)
  //
  // END

  return 0;
}
