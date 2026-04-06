#pragma once

#include <cstddef>
#include "CipherAlgorithm.h"
#include "CipherView.h"
#include "XORCipher.h"


class Cipher {
private:
  Algorithm *cipher_algorithm;
  CipherView ciphered;

  void uncipher_self_into(CipherView&) const;
  void cipher_into_self(ConstCipherView);

public:
  void recipher_and_assign_data(const Cipher&);

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

  // these call a compatibility check algorithm and may uncipher the entire thing
  Cipher& operator+=(const Cipher&);
  Cipher operator+(const Cipher&);

  bool operator==(const Cipher&) const;
  bool operator!=(const Cipher&) const;

  class const_iterator {
  private:
    const Cipher* parent_cipher;
    const char *ciphered_ptr;

  public:
    const_iterator(const Cipher*, const char*);

    char operator*() const;
    char operator[](int idx) const;

    const_iterator& operator+=(size_t);
    const_iterator& operator-=(size_t);

    const_iterator& operator++();
    const_iterator& operator--();
    const_iterator operator++(int);
    const_iterator operator--(int);

    const_iterator operator+(size_t) const;
    const_iterator operator-(size_t) const;

    bool operator==(const const_iterator&) const;
    bool operator!=(const const_iterator&) const;

    bool operator<(const const_iterator&) const;
    bool operator>=(const const_iterator&) const;
    bool operator>(const const_iterator&) const;
    bool operator<=(const const_iterator&) const;

    ptrdiff_t operator-(const const_iterator&) const;
  };
  const_iterator begin() const;
  const_iterator end() const;
};
