#ifndef BOWLINGBALL_HPP
#define BOWLINGBALL_HPP

#include "BowlingObject.hpp"
#include "BowlingPin.hpp"

class BowlingBall : public BowlingObject {
public:
  BowlingBall(GeometryNode *object, float mass);
  ~BowlingBall();

  bool doesCollide(BowlingObject* otherObject);
  void handleCollision(BowlingObject* otherObject);
};

#endif
