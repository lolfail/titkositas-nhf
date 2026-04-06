#pragma once

#include "CipherView.h"

class Algorithm {
public:
  virtual ~Algorithm() {};

  enum class Mode { Encrypt, Decrypt };
  virtual char transform(Mode, char) const = 0;
  virtual void append_ciphered(CipherView&, ConstCipherView) const = 0;
  virtual void append_unciphered(CipherView&, ConstCipherView) const = 0;

  virtual bool is_compatible_with(Algorithm*) const = 0;

  virtual Algorithm* clone() const = 0;
};
