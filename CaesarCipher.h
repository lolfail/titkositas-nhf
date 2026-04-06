#pragma once

#include "CipherAlgorithm.h"
#include "CipherView.h"

class CaesarCipher : public Algorithm {
private:
  int shift;

public:
  explicit CaesarCipher(int);
  virtual ~CaesarCipher() = default;

  char transform(Algorithm::Mode, char) const override;
  // assume destination is large enough, but currently has len for views
  void append_ciphered(CipherView&, ConstCipherView) const override;
  void append_unciphered(CipherView&, ConstCipherView) const override;

  bool is_compatible_with(Algorithm*) const override;

  Algorithm* clone() const override;
};
