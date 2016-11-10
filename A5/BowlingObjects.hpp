#ifndef BOWLING_OBJECTS_HPP
#define BOWLING_OBJECTS_HPP

#include "BowlingBall.hpp"
#include "BowlingPin.hpp"
#include <vector>

// Allow objects to get away from each other after colliding
// so they don't stick to each other
const unsigned int FRAMES_UNTIL_NEW_COLLISION = 0;

// Acceleration due to gravity in feet/second
const float GRAVITY_ACCELERATION = 32.15f;

class BowlingObjects {
  std::vector<BowlingObject *> m_bowlingObjects;
  std::vector<unsigned int> m_lastCollided;

  float m_friction; // coefficient of kinetic friction

public:
  BowlingObjects();
  ~BowlingObjects();

  void addBowlingObject(BowlingObject *bowlingObject);

  float getFriction();
  void setFriction(float friction);

  void setAcceleration(BowlingObject *bowlingObject);

  void resetObjects();

  void setUpLastCollided();
  int lastCollidedIndex(int indexBowlingObject1, int indexBowlingObject2);

  BowlingBall* getBowlingBall(int i);

  bool canCollideAgain(int indexBowlingObject1, int indexBowlingObject2);
  void handleCollision(int indexBowlingObject1, int indexBowlingObject2);
  void moveObjects(float timeInterval);
};

#endif
