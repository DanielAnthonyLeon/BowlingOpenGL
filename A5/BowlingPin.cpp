#include "BowlingPin.hpp"

BowlingPin::BowlingPin(GeometryNode *object) :
  BowlingObject(object, 3.0f + 6.0f/16.0f)
{

}

BowlingPin::~BowlingPin() {

}

void BowlingPin::handleCollision(BowlingObject *otherObject) {
  if (otherObject->getType() == "BowlingPin") {
    
  }
}
