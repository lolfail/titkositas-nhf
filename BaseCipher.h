#pragma once

#include <cstddef>
#include <cstring>

class BaseCipher {
private:
  char *ciphered_data;
  size_t ciphered_len;

public:
  enum class Mode : char { Encrypt, Decrypt };

  BaseCipher();
  // cstring itt nem lehet rögtön, vagy alosztályban, vagy valami trükkös init függvényben...
  virtual ~BaseCipher();

  virtual char transform(Mode, char) const = 0;
  virtual BaseCipher* clone() const = 0;

  BaseCipher(const BaseCipher&);
  BaseCipher& operator=(const BaseCipher&);
  friend void swap(BaseCipher&, BaseCipher&) noexcept;

  BaseCipher& operator=(const char*);

  class iterator {
  private:
    BaseCipher* parent_cipher;
    char *ciphered_ptr;

  public:
    iterator(BaseCipher*, char*);

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
