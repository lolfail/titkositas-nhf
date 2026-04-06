#include <iostream>
#include "Cipher.h"
#include "XORCipher.h"

void log_cipher(Cipher &cipher, const char *testcase = "") {
  std::cout << testcase << std::endl;
  std::cout << '\t';
  for (auto it : cipher)
    std::cout << it;
  std::cout << std::endl;
}

int main() {
  Cipher og_cipher = Cipher(new XORCipher(42));

  og_cipher = "Hello";
  log_cipher(og_cipher, "Did it get set?");

  // this will swap out the algorithm even because of the swap function
  Cipher diff_key_copy(new XORCipher(30));
  diff_key_copy = og_cipher;
  std::cout << diff_key_copy.call() << "!!!";
  log_cipher(diff_key_copy, "Did copy (=) with different key work?");

  Cipher deep_copied_og_cipher = og_cipher;
  deep_copied_og_cipher += " World!";
  log_cipher(deep_copied_og_cipher, "(+=) addition works (modifies)?");

  log_cipher(og_cipher, "Original is still untouched?");

  Cipher addition_cipher = og_cipher + " World++!";
  log_cipher(addition_cipher, "(+) addition worked?");
  log_cipher(og_cipher, "(+) addition worked - original untouched?");

  Cipher hello_cipher(new XORCipher(30));
  Cipher world_cipher = hello_cipher;
  hello_cipher = "Hello";
  world_cipher = " World!";
  Cipher cumulative = hello_cipher + world_cipher;
  log_cipher(cumulative, "(+) addition of classes worked?");
  hello_cipher += world_cipher;
  log_cipher(hello_cipher, "(+=) addition of classes worked?");

  Cipher key2(new XORCipher(2));
  key2 = "key2 ";
  Cipher key3(new XORCipher(3));
  key3 = "key3";
  Cipher keyadded = key2 + key3;
  log_cipher(keyadded, "key 2 and key 3 were added resulting in:");

  // Cipher eq_og(new XORCipher(42));
  // eq_og = "same";
  // Cipher eq_same_key = eq_og;
  // std::cout << "were the two (same key) equal?" << std::endl << '\t' << same1 == same2 << std::endl;
  // Cipher eq_diff_key(new XORCipher(30));
  // eq_diff_key = eq_og;
  // std::cout << "were the two (different key) equal?" << std::endl << '\t' << same1 == same2 << std::endl;
  // std::cout << "were the two (same) equal?" << std::endl << '\t' << same1 == same2 << std::endl;

  return 0;
}
