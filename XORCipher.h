#pragma once

#include "CipherAlgorithm.h"

class XORCipher : public Algorithm {
private:
  unsigned int magic_key;

public:
  explicit XORCipher(unsigned int);
  virtual ~XORCipher() = default;

  char transform(Mode, char) const override;

  Algorithm* clone() const override;
};
