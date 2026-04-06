#include <cstring>
#include <algorithm>
#include "Cipher.h"
#include "CipherAlgorithm.h"
#include "CipherView.h"

Cipher::const_iterator Cipher::begin() const {
  return const_iterator(this, ciphered.data);
}
Cipher::const_iterator Cipher::end() const {
  return const_iterator(this, ciphered.data + ciphered.len);
}

Cipher::Cipher(Algorithm *algo) :
  cipher_algorithm(algo),
  ciphered(nullptr, 0)
{ }
Cipher::~Cipher() {
  delete[] ciphered.data;
  delete cipher_algorithm; // WARNING! if this is shallow copied, this will be nasty
  ciphered.len = 0;
}

Cipher::Cipher(const Cipher &rhs_cipher) {
  ciphered.len = rhs_cipher.ciphered.len;
  ciphered.data = new char[ciphered.len];
  for (size_t i = 0; i < ciphered.len; ++i)
    ciphered.data[i] = rhs_cipher.ciphered.data[i];

  cipher_algorithm = rhs_cipher.cipher_algorithm->clone();
}
void swap(Cipher &a, Cipher &b) noexcept {
  using std::swap;
  swap(a.ciphered.data, b.ciphered.data);
  swap(a.ciphered.len, b.ciphered.len);
  swap(a.cipher_algorithm, b.cipher_algorithm);
}
Cipher& Cipher::operator=(Cipher rhs_cipher) {
  swap(*this, rhs_cipher);
  return *this;
}

Cipher& Cipher::operator=(const char *cstr) {
  delete[] ciphered.data;
  ciphered.len = strlen(cstr);
  ciphered.data = new char[ciphered.len];

  for (size_t i = 0; i < ciphered.len; ++i) {
    ciphered.data[i] = cipher_algorithm->transform(Algorithm::Mode::Encrypt, cstr[i]);
  }
  return *this;
}

Cipher& Cipher::operator+=(const char *cstr) {
  ConstCipherView cstr_cipherview(cstr, strlen(cstr));

  size_t appended_len = ciphered.len + cstr_cipherview.len;
  CipherView ciphered_appended(new char[appended_len], ciphered.len);

  memcpy(ciphered_appended.data, ciphered.data, ciphered.len);
  cipher_algorithm->append_unciphered(ciphered_appended, cstr_cipherview);

  delete[] ciphered.data;
  ciphered = ciphered_appended;
  return *this;
}
Cipher Cipher::operator+(const char *cstr) {
  Cipher cipher_cpy = *this;
  cipher_cpy += cstr;
  return cipher_cpy;
}

Cipher& Cipher::operator+=(const Cipher &rhs_cipher) {
  size_t appended_len = this->ciphered.len + rhs_cipher.ciphered.len;
  CipherView ciphered_appended(new char[appended_len], this->ciphered.len);

  memcpy(ciphered_appended.data, this->ciphered.data, this->ciphered.len);

  bool are_compatible_ciphers = cipher_algorithm->is_compatible_with(rhs_cipher.cipher_algorithm);
  if (are_compatible_ciphers)
    cipher_algorithm->append_ciphered(ciphered_appended, rhs_cipher.ciphered);
  else {
    CipherView unciphered_view(new char[rhs_cipher.ciphered.len], rhs_cipher.ciphered.len);
    for (auto it = rhs_cipher.begin(); it != rhs_cipher.end(); ++it)
      unciphered_view.data[it - rhs_cipher.begin()] = *it;
    cipher_algorithm->append_unciphered(ciphered_appended, unciphered_view);
  }

  delete[] ciphered.data;
  ciphered = ciphered_appended;
  return *this;
}
Cipher Cipher::operator+(const Cipher &rhs_cipher) {
  Cipher cipher_cpy = *this;
  cipher_cpy += rhs_cipher;
  return cipher_cpy;
}

bool Cipher::operator==(const Cipher &rhs_cipher) const {
  bool are_equal_length = this->ciphered.len == rhs_cipher.ciphered.len;
  if (!are_equal_length) return false;

  for (
    auto this_it = this->begin(), rhs_it = rhs_cipher.begin();
    this_it != this->end(); // at this point they must be same length
    ++this_it, ++rhs_it
  ) {
    if (*this_it != *rhs_it) return false;
  }

  return true;
}
bool Cipher::operator!=(const Cipher &rhs_cipher) const {
  return !(*this == rhs_cipher);
}
