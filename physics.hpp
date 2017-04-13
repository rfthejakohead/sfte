#ifndef SFTE_PHYSICS_HPP
#define SFTE_PHYSICS_HPP

#include "core.hpp"
#include "math.hpp"
#include "pointybox/pointybox.hpp"

namespace sfte {
	struct CollisionProperties { // Note: PointyBox used here :)
		pb::AABBVector aabbs;	// This holds AABB data.
		pb::PointVector points;	// This holds point data.
		pb::EdgeVector edges;	// This holds edge data.
	};

	struct PhysicsProperty {
		bool tangible;		// Is the object tangible? If not, don't even bother with collision checking.
		size_t collisionID;	// Where to look at in CollisionProperties when checking collisions.

		PhysicsProperty(bool isTangible = false, size_t collisionPropsID = 0);
	};

	class PhysicsQuad {
		

	public:
		
	};

	class PhysicsPoint {
		

	public:
		
	};
}

#endif