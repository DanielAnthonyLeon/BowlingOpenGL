#ifndef BOWLING_PIN_HPP
#define BOWLING_PIN_HPP

#include "BowlingObject.hpp"

class BowlingPin : public BowlingObject {
public:
  BowlingPin(GeometryNode *object);
  ~BowlingPin();

  void handleCollision(BowlingObject *otherObject);
};

#endif
