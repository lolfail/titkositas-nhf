#include "Cipher.h"
#include "CipherAlgorithm.h"
#include <cstring>
#include <algorithm>

Cipher::iterator Cipher::begin() {
  return iterator(this, ciphered_data);
}
Cipher::iterator Cipher::end() {
  return iterator(this, ciphered_data + ciphered_len);
}

Cipher::Cipher(Algorithm *algo) :
  cipher_algorithm(algo),
  ciphered_len(0),
  ciphered_data(nullptr)
{ }
Cipher::~Cipher() {
  delete[] ciphered_data;
  delete cipher_algorithm; // WARNING! if this is shallow copied, this will be nasty
  ciphered_len = 0;
}

Cipher::Cipher(const Cipher &rhs_cipher) {
  ciphered_len = rhs_cipher.ciphered_len;
  ciphered_data = new char[ciphered_len];
  for (size_t i = 0; i < ciphered_len; ++i)
    ciphered_data[i] = rhs_cipher.ciphered_data[i];

  cipher_algorithm = rhs_cipher.cipher_algorithm->clone();
}
void swap(Cipher &a, Cipher &b) noexcept {
  using std::swap;
  swap(a.ciphered_data, b.ciphered_data);
  swap(a.ciphered_len, b.ciphered_len);
  swap(a.cipher_algorithm, b.cipher_algorithm);
}
Cipher& Cipher::operator=(Cipher rhs_cipher) {
  swap(*this, rhs_cipher);
  return *this;
}

Cipher& Cipher::operator=(const char *cstr) {
  delete[] ciphered_data;
  ciphered_len = strlen(cstr);
  ciphered_data = new char[ciphered_len];

  for (size_t i = 0; i < ciphered_len; ++i) {
    ciphered_data[i] = cipher_algorithm->transform(Algorithm::Mode::Encrypt, cstr[i]);
  }
  return *this;
}

Cipher& Cipher::operator+=(const char *cstr) {
  size_t cstr_len = strlen(cstr);

  size_t ciphered_len_extended = ciphered_len + cstr_len;
  char *ciphered_data_extended = new char[ciphered_len_extended];

  memcpy(ciphered_data_extended, ciphered_data, ciphered_len);
  for (size_t i = 0; i < cstr_len; ++i)
    ciphered_data_extended[i+ciphered_len] = cipher_algorithm->transform(Algorithm::Mode::Encrypt, cstr[i]);

  delete[] ciphered_data;
  ciphered_data = ciphered_data_extended;
  ciphered_len = ciphered_len_extended;
  return *this;
}
Cipher Cipher::operator+(const char *cstr) {
  Cipher cipher_cpy = *this;
  cipher_cpy += cstr;
  return cipher_cpy;
}
