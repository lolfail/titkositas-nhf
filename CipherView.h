#pragma once

#include <cstddef>

struct CipherView {
  char *data;
  size_t len;

  CipherView(char *d = nullptr, size_t l = 0) : data(d), len(l) {}
};
struct ConstCipherView {
  const char *data;
  size_t len;

  ConstCipherView(const char *d = nullptr, size_t l = 0) : data(d), len(l) {}
  ConstCipherView(const CipherView &view) : data(view.data), len(view.len) {}
};
