#include <cstring>
#include <algorithm>
#include <stdexcept>

#include "Cipher.h"
#include "CipherAlgorithm.h"
#include "CipherView.h"

Cipher::const_iterator Cipher::begin() const {
  return const_iterator(this, ciphered.data);
}
Cipher::const_iterator Cipher::end() const {
  return const_iterator(this, ciphered.data + ciphered.len);
}

// assume destination is large enough
void Cipher::uncipher_self_into(CipherView &destination_view) const {
  for (auto it = this->begin(); it != this->end(); ++it) {
    size_t index = it - this->begin();
    destination_view.data[index] = *it;
  }
  destination_view.len = this->ciphered.len;
}
void Cipher::cipher_into_self(ConstCipherView source_view) {
  // buffered because we do not know wheter underlying algorithm depends on previous characters
  CipherView buffer(new char[ciphered.len], 0);
  cipher_algorithm->append_unciphered(buffer, source_view);

  delete[] ciphered.data;
  ciphered = buffer;
}

void Cipher::recipher_and_assign_data(const Cipher &source_cipher) {
  if (this == &source_cipher) return;

  CipherView unciphered_view(new char[source_cipher.ciphered.len], source_cipher.ciphered.len);
  source_cipher.uncipher_self_into(unciphered_view);
  this->cipher_into_self(unciphered_view);
  delete[] unciphered_view.data;
}

Cipher::Cipher(Algorithm *algo) :
  cipher_algorithm(algo),
  ciphered(nullptr, 0),
  plaintext_buffer(nullptr)
{ }
Cipher::~Cipher() {
  delete[] ciphered.data;
  ciphered.len = 0;

  delete[] plaintext_buffer;
}

Cipher::Cipher(const Cipher &rhs_cipher) :
  cipher_algorithm(rhs_cipher.cipher_algorithm->clone()),
  ciphered(new char[rhs_cipher.ciphered.len], rhs_cipher.ciphered.len),
  plaintext_buffer(nullptr)
{
  // copy only ciphered data
  for (size_t i = 0; i < ciphered.len; ++i)
    ciphered.data[i] = rhs_cipher.ciphered.data[i];
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
  ConstCipherView cstr_cipherview(cstr, strlen(cstr));
  cipher_into_self(cstr_cipherview);
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
  size_t appended_allocated_size = this->ciphered.len + rhs_cipher.ciphered.len;
  CipherView ciphered_appended(new char[appended_allocated_size], this->ciphered.len);

  memcpy(ciphered_appended.data, this->ciphered.data, this->ciphered.len);

  bool can_append_without_unciphering = cipher_algorithm->is_compatible_with(rhs_cipher.cipher_algorithm);
  if (can_append_without_unciphering)
    cipher_algorithm->append_ciphered(ciphered_appended, rhs_cipher.ciphered);
  else {
    CipherView unciphered_view(new char[rhs_cipher.ciphered.len], 0);
    rhs_cipher.uncipher_self_into(unciphered_view);
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

char Cipher::operator[](size_t idx) const {
  if (idx >= ciphered.len)
    throw std::out_of_range("Provided index was not within the data.");
  return cipher_algorithm->transform(Algorithm::Mode::Decrypt, ciphered.data[idx]);
}

bool Cipher::operator==(const Cipher &rhs_cipher) const {
  bool are_equal_length = this->ciphered.len == rhs_cipher.ciphered.len;
  if (!are_equal_length) return false;

  for (size_t i = 0; i < this->ciphered.len; ++i) {
    if ((*this)[i] != rhs_cipher[i]) return false;
  }

  return true;
}
bool Cipher::operator!=(const Cipher &rhs_cipher) const {
  return !(*this == rhs_cipher);
}
bool Cipher::operator==(const char *rhs_cstr) const {
  bool are_equal_length = this->ciphered.len == strlen(rhs_cstr);
  if (!are_equal_length) return false;

  for (size_t i = 0; i < this->ciphered.len; ++i) {
    if ((*this)[i] != rhs_cstr[i]) return false;
  }

  return true;
}
bool Cipher::operator!=(const char *rhs_cstr) const {
  return !(*this == rhs_cstr);
}

char* Cipher::c_str() const {
  size_t allocated = ciphered.len + 1;
  CipherView unciphered_view = CipherView(new char[allocated], allocated);

  this->uncipher_self_into(unciphered_view);
  unciphered_view.data[unciphered_view.len] = '\0';

  delete[] plaintext_buffer;
  plaintext_buffer = unciphered_view.data;
  return plaintext_buffer;
}
