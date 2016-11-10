#include "BowlingObject.hpp"
#include <iostream>

using namespace irrklang;

BowlingObject::BowlingObject(GeometryNode *object, float mass) :
  m_object(object),
  m_mass(mass),
  m_gotHit(false)
{
  m_originalTrans = object->get_transform();
  m_centre = m_originalTrans * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  m_velocity = glm::vec4(0.0f);
  m_acceleration = 0.0f;

  m_radius = object->get_scaleAmount().x;
  m_height = object->get_scaleAmount().y;

  m_soundEngine = createIrrKlangDevice();
  assert(m_soundEngine);
  m_soundEngine->setListenerPosition(irrklang::vec3df(0, 0, 0), irrklang::vec3df(0, 0, -1));
}

BowlingObject::~BowlingObject() {
  delete m_soundEngine;
}

std::string BowlingObject::getType() {
  std::string name = m_object->m_name;
  int l = name.length();
  assert(l > 0);
  if (name.substr(0, 11) == "BowlingBall")
    return "BowlingBall";
  else if (name.substr(0, 10) == "BowlingPin")
    return "BowlingPin";
  else
    return "Other";
}

glm::vec4 BowlingObject::getCentre() {
  return m_centre;
}

float BowlingObject::getRadius() {
  return m_radius;
}

float BowlingObject::getHeight() {
  return m_height;
}

glm::vec4 BowlingObject::getVelocity() {
  return m_velocity;
}

glm::vec4 BowlingObject::getAngularVelocity() {
  glm::vec4 w = m_velocity / m_radius;
  return w;
}

float BowlingObject::getAcceleration() {
  return m_acceleration;
}

float BowlingObject::getMass() {
  return m_mass;
}

void BowlingObject::resetObject() {
  m_object->set_transform(m_originalTrans);
  m_centre = m_originalTrans * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
  m_velocity = glm::vec4(0.0f);
}

void BowlingObject::setVelocity(glm::vec4 velocity) {
  m_velocity = velocity;
}

void BowlingObject::updateVelocity(float timeInterval) {
  float velocityMagnitude = glm::length(m_velocity);
  if (velocityMagnitude - timeInterval * m_acceleration < 0.000001)
    m_velocity = glm::vec4(0.0f);
  else {
    glm::vec4 accelerationDirection = -glm::normalize(m_velocity);
    m_velocity += timeInterval * m_acceleration * accelerationDirection;
  }
  // Might bounce off side of lane
  if (velocityMagnitude > 0.000001)
    bounceOffSide();
}

void BowlingObject::setAcceleration(float acceleration) {
  m_acceleration = acceleration;
}

bool BowlingObject::hasBeenHit() {
  return m_gotHit;
}

void BowlingObject::gotHit() {
  m_gotHit = true;
}

void BowlingObject::displaceObject(float timeInterval) {
  glm::vec4 displacement = m_velocity * timeInterval;
  m_centre += displacement;
  m_object->translate(glm::vec3(displacement));
}

void BowlingObject::rotateObject(glm::vec3 axis, float angle) {
  m_object->rotate(glm::vec3(m_centre), axis, angle);
}

void BowlingObject::rollObject(float timeInterval) {
  float dTheta = timeInterval * glm::length(getAngularVelocity());
  glm::vec3 up(0.0f, 1.0f, 0.0f);

  if (glm::length(m_velocity) > 0.000001) {
    glm::vec3 normalizedVelocity = glm::normalize(glm::vec3(m_velocity));
    glm::vec3 rotationAxis = glm::cross(up, normalizedVelocity);
    rotateObject(rotationAxis, dTheta);
  }
}

// Default bounding box collision
bool BowlingObject::doesCollide(BowlingObject* otherObject) {
  float xMin = m_centre.x - m_radius;
  float xMax = m_centre.x + m_radius;
  float yMin = m_centre.y - m_height/2;
  float yMax = m_centre.y + m_height/2;
  float zMin = m_centre.z - m_radius;
  float zMax = m_centre.z + m_radius;

  float xMinOther = otherObject->getCentre().x - otherObject->getRadius();
  float xMaxOther = otherObject->getCentre().x + otherObject->getRadius();
  float yMinOther = otherObject->getCentre().y - otherObject->getHeight()/2;
  float yMaxOther = otherObject->getCentre().y + otherObject->getHeight()/2;
  float zMinOther = otherObject->getCentre().z - otherObject->getRadius();
  float zMaxOther = otherObject->getCentre().z + otherObject->getRadius();

  bool above = yMin > yMaxOther;
  bool below = yMax < yMinOther;
  bool inFront = zMin > zMaxOther;
  bool behind = zMax < zMinOther;
  bool toTheRight = xMin > xMaxOther;
  bool toTheLeft = xMax < xMinOther;

  bool collide = !(above || below || inFront || behind || toTheRight || toTheLeft);

  return collide;
}

void BowlingObject::moveObject(float timeInterval) {
  updateVelocity(timeInterval); // accelerate object
  displaceObject(timeInterval); // move object
  rollObject(timeInterval); // rotate object
}

void BowlingObject::bounceOffSide() {
  glm::vec3 v = glm::normalize(glm::vec3(m_velocity));
  float magnitude = glm::length(m_velocity);
  if (m_centre.x <= LANE_LEFTSIDE) {
    glm::vec3 N = glm::vec3(1.0f, 0.0, 0.0f);
    glm::vec3 vr = v - 2 * glm::dot(v, N) * N;
    m_velocity = glm::vec4(magnitude * vr, 0.0f);
  }
  else if (m_centre.x >= LANE_RIGHTSIDE) {
    glm::vec3 N = glm::vec3(-1.0f, 0.0, 0.0f);
    glm::vec3 vr = v - 2 * glm::dot(v, N) * N;
    m_velocity = glm::vec4(magnitude * vr, 0.0f);
  }
  else if (m_centre.z >= LANE_FRONT) {
    glm::vec3 N = glm::vec3(0.0f, 0.0, -1.0f);
    glm::vec3 vr = v - 2 * glm::dot(v, N) * N;
    m_velocity = glm::vec4(magnitude * vr, 0.0f);
  }
  else if (m_centre.z <= LANE_BACK) {
    glm::vec3 N = glm::vec3(0.0f, 0.0, 1.0f);
    glm::vec3 vr = v - 2 * glm::dot(v, N) * N;
    m_velocity = glm::vec4(magnitude * vr, 0.0f);
  }
}
