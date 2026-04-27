#include <cstring>

#include "memtrace.h"

#include "CipherAlgorithm.h"
#include "CipherView.h"
#include "CaesarCipher.h"

CaesarCipher::CaesarCipher(int shift) : shift(shift) {}

char CaesarCipher::transform(Algorithm::Mode cipher_mode, char unciphered_char) const {
  int cipher = unciphered_char;
  if (cipher_mode == Algorithm::Mode::Encrypt)
    return (cipher + shift) % 256;
  else
    return (cipher - shift) % 256;
}

// these append instead of filling up the respective view from start,
// because some algorithms have to know the position
void CaesarCipher::append_ciphered(CipherView &destination, ConstCipherView source) const {
  for (size_t i = 0; i < source.len; ++i)
    destination.data[i + destination.len] = source.data[i];
  destination.len = destination.len + source.len;
}
void CaesarCipher::append_unciphered(CipherView &destination, ConstCipherView source) const {
  for (size_t i = 0; i < source.len; ++i)
    destination.data[i + destination.len] = transform(Algorithm::Mode::Encrypt, source.data[i]);
  destination.len = destination.len + source.len;
}

bool CaesarCipher::is_compatible_with(Algorithm* other_cipher) const {
  // dynamic cast then check whether algo specific things match (key, for caesar shift)
  // then cipher shall decipher if this is false and then append unciphered
  CaesarCipher* cast_other_cipher = dynamic_cast<CaesarCipher*>(other_cipher);
  if (cast_other_cipher == nullptr) return false;

  return cast_other_cipher->shift == this->shift;
}

Algorithm* CaesarCipher::clone() const {
  return new CaesarCipher(shift);
}
