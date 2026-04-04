#include "XORCipher.h"
#include "CipherAlgorithm.h"

XORCipher::XORCipher(unsigned int key) : magic_key(key) {}

char XORCipher::transform(Mode, char from_ch) const {
  return from_ch ^ magic_key;
}

Algorithm* XORCipher::clone() const {
  return new XORCipher(magic_key);
}
