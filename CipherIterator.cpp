#include "Cipher.h"
#include "CipherAlgorithm.h"
#include <cstddef>

using const_iterator = Cipher::const_iterator;

const_iterator::const_iterator(const Cipher* parent_ptr, const char *pointed_char) :
  parent_cipher(parent_ptr),
  ciphered_ptr(pointed_char)
{}

char const_iterator::operator*() const {
  using Mode = Algorithm::Mode;
  return parent_cipher->cipher_algorithm->transform(Mode::Decrypt, *ciphered_ptr);
}
char const_iterator::operator[](int idx) const {
  return *(*this + idx);
}

const_iterator& const_iterator::operator+=(size_t rhs) {
  ciphered_ptr += rhs;
  return *this;
}
const_iterator& const_iterator::operator-=(size_t rhs) {
  ciphered_ptr -= rhs;
  return *this;
}

const_iterator& const_iterator::operator++() {
  ++ciphered_ptr;
  return *this;
}
const_iterator& const_iterator::operator--() {
  ++ciphered_ptr;
  return *this;
}
const_iterator const_iterator::operator++(int) {
  auto tmp = *this;
  ++ciphered_ptr;
  return tmp;
}
const_iterator const_iterator::operator--(int) {
  auto tmp = *this;
  --ciphered_ptr;
  return tmp;
}

const_iterator const_iterator::operator+(size_t rhs) const {
  return const_iterator(parent_cipher, ciphered_ptr + rhs);
}
const_iterator const_iterator::operator-(size_t rhs) const {
  return const_iterator(parent_cipher, ciphered_ptr - rhs);
}

bool const_iterator::operator==(const const_iterator &rhs_it) const {
  return this->ciphered_ptr == rhs_it.ciphered_ptr;
}
bool const_iterator::operator!=(const const_iterator &rhs_it) const {
  return !(*this == rhs_it);
}

bool const_iterator::operator<(const const_iterator &rhs_it) const {
  return this->ciphered_ptr < rhs_it.ciphered_ptr;
}
bool const_iterator::operator>=(const const_iterator &rhs_it) const {
  return !(*this < rhs_it);
}
bool const_iterator::operator>(const const_iterator &rhs_it) const {
  return rhs_it < *this;
}
bool const_iterator::operator<=(const const_iterator &rhs_it) const {
  return rhs_it >= *this;
}

ptrdiff_t const_iterator::operator-(const const_iterator &rhs) const {
  return this->ciphered_ptr - rhs.ciphered_ptr;
}
