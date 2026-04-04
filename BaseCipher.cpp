#include "BaseCipher.h"
#include <cstring>
#include <algorithm>

BaseCipher::iterator BaseCipher::begin() {
  return iterator(this, ciphered_data);
}
BaseCipher::iterator BaseCipher::end() {
  return iterator(this, ciphered_data + ciphered_len);
}

BaseCipher::BaseCipher() : ciphered_len(0) {}

void swap(BaseCipher& a, BaseCipher& b) noexcept {
  using std::swap;
  swap(a.ciphered_data, b.ciphered_data);
  swap(a.ciphered_len, b.ciphered_len);
};
BaseCipher::BaseCipher(const BaseCipher& rhs_bc) {
  BaseCipher *rhs_copy = rhs_bc.clone();
  swap(*this, *rhs_copy);
  delete rhs_copy;
}
BaseCipher& BaseCipher::operator=(const BaseCipher &rhs_bc) {
  BaseCipher *rhs_copy = rhs_bc.clone();
  swap(*this, *rhs_copy);
  delete rhs_copy;
  return *this;
}

BaseCipher& BaseCipher::operator=(const char *cstr) {
  delete ciphered_data;
  ciphered_len = strlen(cstr);
  ciphered_data = new char[ciphered_len];

  for (size_t i = 0; i < ciphered_len; ++i)
    ciphered_data[i] = transform(Mode::Encrypt, cstr[i]);

  return *this;
}
