#pragma once

class Algorithm {
public:
  virtual ~Algorithm() {};

  enum class Mode : char { Encrypt, Decrypt };
  virtual char transform(Mode, char) const = 0;

  virtual Algorithm* clone() const = 0;
};
