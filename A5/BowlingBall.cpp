#include "BowlingBall.hpp"

BowlingBall::BowlingBall(GeometryNode *object, float mass) :
  BowlingObject(object, mass)
{
}

BowlingBall::~BowlingBall() {

}

bool BowlingBall::doesCollide(BowlingObject* otherObject) {
  if (BowlingObject::doesCollide(otherObject)) {
    if (otherObject->getType() == "BowlingBall") {
      float centreDistance = glm::length(m_centre - otherObject->getCentre());
      float radiusSum = m_radius + otherObject->getRadius();
      return centreDistance < radiusSum;
    }

    else if (otherObject->getType() == "BowlingPin")
      return true;
  }
  return false;
}

void BowlingBall::handleCollision(BowlingObject* otherObject) {
  glm::vec4 x1 = m_centre;
  glm::vec4 v1 = m_velocity;
  float m1 = m_mass;
  glm::vec4 x2 = otherObject->getCentre();
  glm::vec4 v2 = otherObject->getVelocity();
  float m2 = otherObject->getMass();

  // By the time the collision is realized, the balls could be intersecting quite a bit.
  // This pretends that the balls are just touching
  glm::vec4 centreDifference = (m_radius + otherObject->getRadius()) * glm::normalize(x1 - x2);
  centreDifference.y = 0; // For pins

  if (otherObject->getType() == "BowlingBall") {
    glm::vec4 v1_after = v1 - (2*m2/(m1+m2)) * glm::dot(v1-v2, centreDifference)/glm::pow(glm::length(centreDifference), 2.0f) * centreDifference;
    glm::vec4 v2_after = v2 - (2*m1/(m1+m2)) * glm::dot(v2-v1, -centreDifference)/glm::pow(glm::length(-centreDifference), 2.0f) * (-centreDifference);

    m_velocity = v1_after;
    otherObject->setVelocity(v2_after);

    glm::vec4 collisionPos = m_centre + m_radius * centreDifference;
    irrklang::vec3df collisionPos3df(collisionPos.x, collisionPos.y, collisionPos.z);
    m_soundEngine->play3D("Assets/bounce.wav", collisionPos3df);
  }
  else if (otherObject->getType() == "BowlingPin") {
    glm::vec4 v1_after = v1 - (2*m2/(m1+m2)) * glm::dot(v1-v2, centreDifference)/glm::pow(glm::length(centreDifference), 2.0f) * centreDifference;
    glm::vec4 v2_after = v2 - (2*m1/(m1+m2)) * glm::dot(v2-v1, -centreDifference)/glm::pow(glm::length(-centreDifference), 2.0f) * (-centreDifference);
    otherObject->rotateObject(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(90.0f));

    m_velocity = v1_after;

    irrklang::vec3df collisionPos3df(m_centre.x, m_centre.y, m_centre.z);
    m_soundEngine->play3D("Assets/strike.wav", collisionPos3df);
  }
}
