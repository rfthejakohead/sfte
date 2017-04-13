#include "physics.hpp"

sfte::PhysicsProperty::PhysicsProperty(bool isTangible, size_t collisionPropsID) :
	tangible(isTangible),
	collisionID(collisionPropsID)
{ }