#pragma once

#include "CipherAlgorithm.h"
#include <cstddef>

class Cipher {
private:
  Algorithm *cipher_algorithm;
  size_t ciphered_len;
  char *ciphered_data;

public:
  Cipher(Algorithm*);
  ~Cipher();

  // Copying algorithms all deep copy, so that we can avoid double frees and a reference counter.
  // However, it may be worthwhile to be able to specify via a macro whether to shallow or deep copy...
  Cipher(const Cipher&);
  friend void swap(Cipher&, Cipher&) noexcept;
  Cipher& operator=(const Cipher);

  Cipher& operator=(const char*);

  Cipher& operator+=(const char*);
  Cipher operator+(const char*);

  // should these require the entire thing to be recrypted? (context dependant?)
  // a function defined such as is_context_dependant could help... or variable?
  // for vigenére, the rhs needs to be unencrypted either way, but lhs does not.
  // such an algorithm where lhs needs decrypting is context dependant ...
  // but then do we need tracking for three states (enum into algo??) (decrypt: nothing, rhs only, lhs + rhs both)?
  Cipher& operator+=(const Cipher&);
  Cipher operator+(const Cipher&);

  class iterator {
  private:
    Cipher* parent_cipher;
    char *ciphered_ptr;

  public:
    iterator(Cipher*, char*);

    char operator*() const;
    char operator[](int idx) const;

    iterator& operator+=(size_t);
    iterator& operator-=(size_t);

    iterator& operator++();
    iterator& operator--();
    iterator operator++(int);
    iterator operator--(int);

    iterator operator+(size_t) const;
    iterator operator-(size_t) const;

    bool operator==(const iterator&) const;
    bool operator!=(const iterator&) const;

    bool operator<(const iterator&) const;
    bool operator>=(const iterator&) const;
    bool operator>(const iterator&) const;
    bool operator<=(const iterator&) const;

    ptrdiff_t operator-(const iterator&) const;
  };
  iterator begin();
  iterator end();
};
