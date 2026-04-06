#include "XORCipher.h"
#include "CipherAlgorithm.h"
#include "CipherView.h"
#include <cstring>

XORCipher::XORCipher(unsigned int key) : magic_key(key) {}

char XORCipher::transform(Algorithm::Mode, char from_ch) const {
  return from_ch ^ magic_key;
}

// these append instead of filling up the respective view from start,
// because some algorithms have to know the position
void XORCipher::append_ciphered(CipherView &destination, ConstCipherView source) const {
  for (size_t i = 0; i < source.len; ++i)
    destination.data[i + destination.len] = source.data[i];
  destination.len = destination.len + source.len;
}
void XORCipher::append_unciphered(CipherView &destination, ConstCipherView source) const {
  for (size_t i = 0; i < source.len; ++i)
    destination.data[i + destination.len] = transform(Algorithm::Mode::Encrypt, source.data[i]);
  destination.len = destination.len + source.len;
}

bool XORCipher::is_compatible_with(Algorithm* other_cipher) const {
  // dynamic cast then check whether algo specific things match (key, for caesar shift)
  // then cipher shall decipher if this is false and then append unciphered
  XORCipher* cast_other_cipher = dynamic_cast<XORCipher*>(other_cipher);
  if (cast_other_cipher == nullptr) return false;

  return cast_other_cipher->magic_key == this->magic_key;
}

Algorithm* XORCipher::clone() const {
  return new XORCipher(magic_key);
}
