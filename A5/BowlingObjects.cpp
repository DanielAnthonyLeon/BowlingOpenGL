#include "BowlingObjects.hpp"

BowlingObjects::BowlingObjects() :
  m_friction(0.0f)
{

}

BowlingObjects::~BowlingObjects() {
  for (int i = 0; i < m_bowlingObjects.size(); i++)
    delete m_bowlingObjects.at(i);
}

void BowlingObjects::addBowlingObject(BowlingObject *bowlingObject) {
  m_bowlingObjects.push_back(bowlingObject);
}

float BowlingObjects::getFriction() {
  return m_friction;
}

void BowlingObjects::setFriction(float friction) {
  m_friction = friction;
  // Apply new acceleration for objects based on the friction
  for (int i = 0; i < m_bowlingObjects.size(); i++) {
    setAcceleration(m_bowlingObjects.at(i));
  }
}

void BowlingObjects::setAcceleration(BowlingObject *bowlingObject) {
  float acceleration = m_friction * GRAVITY_ACCELERATION;
  bowlingObject->setAcceleration(acceleration);
}

void BowlingObjects::resetObjects() {
  for (int i = 0; i < m_bowlingObjects.size(); i++)
    m_bowlingObjects.at(i)->resetObject();
}

void BowlingObjects::setUpLastCollided() {
  int size = m_bowlingObjects.size();
  for (int i = 0; i < size*size; i++)
    m_lastCollided.push_back(FRAMES_UNTIL_NEW_COLLISION);
}

int BowlingObjects::lastCollidedIndex(int indexBowlingObject1, int indexBowlingObject2) {
  int size = m_bowlingObjects.size();
  int index = indexBowlingObject1 * size + indexBowlingObject2;
}

BowlingBall* BowlingObjects::getBowlingBall(int i) {
  assert(i < m_bowlingObjects.size());
  BowlingObject *bowlingObject = m_bowlingObjects.at(i);
  assert(bowlingObject->getType() == "BowlingBall");
  BowlingBall *bowlingBall = static_cast<BowlingBall *>(bowlingObject);
  return bowlingBall;
}

bool BowlingObjects::canCollideAgain(int indexBowlingObject1, int indexBowlingObject2) {
  unsigned int lastCollided = m_lastCollided.at(lastCollidedIndex(indexBowlingObject1, indexBowlingObject2));
  return lastCollided >= FRAMES_UNTIL_NEW_COLLISION;
}

void BowlingObjects::handleCollision(int indexBowlingObject1, int indexBowlingObject2) {
  BowlingObject *bowlingObject1 = m_bowlingObjects.at(indexBowlingObject1);
  BowlingObject *bowlingObject2 = m_bowlingObjects.at(indexBowlingObject2);

  // Must wait for objects to get away from each other
  if (bowlingObject1->doesCollide(bowlingObject2)) {
    if (canCollideAgain(indexBowlingObject1, indexBowlingObject2))
      bowlingObject1->handleCollision(bowlingObject2);
    m_lastCollided.at(lastCollidedIndex(indexBowlingObject1, indexBowlingObject2)) = 0;
  }
  // Increment the number of frames since last collided
  else
    m_lastCollided.at(lastCollidedIndex(indexBowlingObject1, indexBowlingObject2))++;
}

void BowlingObjects::moveObjects(float timeInterval) {
  for (int i = 0; i < m_bowlingObjects.size(); i++) {
    BowlingObject *bowlingObjecti = m_bowlingObjects.at(i);
    // See which other objects it is colliding with
    for (int j = i+1; j < m_bowlingObjects.size(); j++) {
      BowlingObject *bowlingObjectj = m_bowlingObjects.at(j);
      handleCollision(i, j);
    }
    bowlingObjecti->moveObject(timeInterval);
  }
}
