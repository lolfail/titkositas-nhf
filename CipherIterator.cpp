#include "Cipher.h"
#include "CipherAlgorithm.h"
#include <cstddef>

using iterator = Cipher::iterator;

iterator::iterator(Cipher* parent_ptr, char *pointed_char) :
  parent_cipher(parent_ptr),
  ciphered_ptr(pointed_char)
{}

char iterator::operator*() const {
  using Mode = Algorithm::Mode;
  return parent_cipher->cipher_algorithm->transform(Mode::Decrypt, *ciphered_ptr);
}
char iterator::operator[](int idx) const {
  return *(*this + idx);
}

iterator& iterator::operator+=(size_t rhs) {
  ciphered_ptr += rhs;
  return *this;
}
iterator& iterator::operator-=(size_t rhs) {
  ciphered_ptr -= rhs;
  return *this;
}

iterator& iterator::operator++() {
  ciphered_ptr++;
  return *this;
}
iterator& iterator::operator--() {
  ciphered_ptr--;
  return *this;
}
iterator iterator::operator++(int) {
  auto tmp = *this;
  ciphered_ptr++;
  return tmp;
}
iterator iterator::operator--(int) {
  auto tmp = *this;
  ciphered_ptr--;
  return tmp;
}

iterator iterator::operator+(size_t rhs) const {
  return iterator(parent_cipher, ciphered_ptr + rhs);
}
iterator iterator::operator-(size_t rhs) const {
  return iterator(parent_cipher, ciphered_ptr - rhs);
}

bool iterator::operator==(const iterator &rhs_it) const {
  return this->ciphered_ptr == rhs_it.ciphered_ptr;
}
bool iterator::operator!=(const iterator &rhs_it) const {
  return !(*this == rhs_it);
}

bool iterator::operator<(const iterator &rhs_it) const {
  return this->ciphered_ptr < rhs_it.ciphered_ptr;
}
bool iterator::operator>=(const iterator &rhs_it) const {
  return !(*this < rhs_it);
}
bool iterator::operator>(const iterator &rhs_it) const {
  return rhs_it < *this;
}
bool iterator::operator<=(const iterator &rhs_it) const {
  return rhs_it >= *this;
}

ptrdiff_t iterator::operator-(const iterator &rhs) const {
  return this->ciphered_ptr - rhs.ciphered_ptr;
}
