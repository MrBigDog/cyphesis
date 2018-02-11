// AUTOGENERATED file, created by the tool generate_stub.py, don't edit!
// If you want to add your own functionality, instead edit the stubMovement_custom.h file.

#include "rulesets/Movement.h"
#include "stubMovement_custom.h"

#ifndef STUB_RULESETS_MOVEMENT_H
#define STUB_RULESETS_MOVEMENT_H

#ifndef STUB_Movement_Movement
//#define STUB_Movement_Movement
   Movement::Movement(LocatedEntity & body)
  {
    
  }
#endif //STUB_Movement_Movement

#ifndef STUB_Movement_reset
//#define STUB_Movement_reset
  void Movement::reset()
  {
    
  }
#endif //STUB_Movement_reset

#ifndef STUB_Movement_updateNeeded
//#define STUB_Movement_updateNeeded
  bool Movement::updateNeeded(const Location & location) const
  {
    return false;
  }
#endif //STUB_Movement_updateNeeded

#ifndef STUB_Movement_getTickAddition
//#define STUB_Movement_getTickAddition
  double Movement::getTickAddition(const Point3D & coordinates, const Vector3D & velocity) const
  {
    return 0;
  }
#endif //STUB_Movement_getTickAddition

#ifndef STUB_Movement_getUpdatedLocation
//#define STUB_Movement_getUpdatedLocation
  int Movement::getUpdatedLocation(Location & return_location)
  {
    return 0;
  }
#endif //STUB_Movement_getUpdatedLocation

#ifndef STUB_Movement_generateMove
//#define STUB_Movement_generateMove
  Atlas::Objects::Operation::RootOperation Movement::generateMove(const Location & new_location)
  {
    return *static_cast<Atlas::Objects::Operation::RootOperation*>(nullptr);
  }
#endif //STUB_Movement_generateMove


#endif