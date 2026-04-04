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

  Cipher deep_copied_og_cipher = og_cipher;
  deep_copied_og_cipher += " World!";
  log_cipher(deep_copied_og_cipher, "(+=) addition works (modifies)?");

  log_cipher(og_cipher, "Original is still untouched?");

  Cipher addition_cipher = og_cipher + " World++!";
  log_cipher(addition_cipher, "(+) addition worked?");
  log_cipher(og_cipher, "(+) addition worked - original untouched?");

  return 0;
}
