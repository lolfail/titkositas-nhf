#include "XORCipher.h"
#include "BaseCipher.h"

XORCipher::XORCipher(unsigned int key) : magic_key(key) {}

char XORCipher::transform(Mode, char from_ch) const {
  return from_ch ^ magic_key;
}

BaseCipher* XORCipher::clone() const {
  BaseCipher *cloned = new XORCipher(*this);
  return cloned;
}
