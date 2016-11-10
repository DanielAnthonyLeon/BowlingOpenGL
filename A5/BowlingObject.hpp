#ifndef BOWLING_OBJECT_HPP
#define BOWLING_OBJECT_HPP

#include "GeometryNode.hpp"
#include <glm/glm.hpp>
#include <string>
#include <cctype>
#include <irrklang/irrKlang.h>

const float LANE_LEFTSIDE = -2.917;
const float LANE_RIGHTSIDE = 2.917;
const float LANE_FRONT = -5.0;
const float LANE_BACK = -45.0;

class BowlingObject {
protected:
  GeometryNode *m_object;

  glm::mat4 m_originalTrans;
  glm::vec4 m_centre;
  float m_radius;
  float m_height;

  glm::vec4 m_velocity;
  float m_acceleration; // Acceleration is in opposite direction of velocity
  float m_mass;

  bool m_gotHit;

  irrklang::ISoundEngine *m_soundEngine;

public:
  BowlingObject(GeometryNode *object, float mass);
  ~BowlingObject();

  std::string getType();
  glm::vec4 getCentre();
  float getRadius();
  float getHeight();
  glm::vec4 getVelocity();
  glm::vec4 getAngularVelocity();
  float getAcceleration();
  float getMass();

  void setVelocity(glm::vec4 velocity);
  void updateVelocity(float timeInterval);
  void setAcceleration(float acceleration);

  void resetObject();

  bool hasBeenHit();
  void gotHit();

  void displaceObject(float timeInterval);
  void rotateObject(glm::vec3 axis, float angle);
  void rollObject(float timeInterval);

  virtual bool doesCollide(BowlingObject* otherObject);
  virtual void handleCollision(BowlingObject* otherObject) =0;
  void moveObject(float timeInterval);

  void bounceOffSide();
};

#endif
