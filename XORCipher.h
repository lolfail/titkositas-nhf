#pragma once

#include "BaseCipher.h"

class XORCipher : public BaseCipher {
private:
  unsigned int magic_key;

public:
  explicit XORCipher(unsigned int);
  ~XORCipher();

  char transform(Mode, char) const override;
  BaseCipher* clone() const override;
};
