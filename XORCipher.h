#pragma once

#include "CipherAlgorithm.h"
#include "CipherView.h"

class XORCipher : public Algorithm {
private:
  unsigned int magic_key;

public:
  explicit XORCipher(unsigned int);
  virtual ~XORCipher() = default;

  char transform(Mode, char) const override;
  // assume destination is large enough, but currently has len for views
  void append_ciphered(CipherView&, ConstCipherView) const override;
  void append_unciphered(CipherView&, ConstCipherView) const override;

  bool is_compatible_with(Algorithm*) const override;

  Algorithm* clone() const override;

  // TODO! delete
  unsigned int call() const { return magic_key; };
};
