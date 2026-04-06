#include <iostream>
#include "CaesarCipher.h"
#include "Cipher.h"
#include "CipherAlgorithm.h"
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

#define CIPHERALGO XORCipher
int main() {
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
