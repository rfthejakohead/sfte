#ifndef SFTE_PHYSICS_HPP
#define SFTE_PHYSICS_HPP

#include "core.hpp"

namespace sfte {
	struct CollisionProperties {
		std::vector < std::vector < std::vector < sf::FloatRect > > > collisionBoxes;	// This holds all collision boxes of a tile. Structure is vec[tileID][bitmask][boxn]
		std::vector < std::vector < std::vector < sf::Vector2f > > > corners;			// This holds all corners of tile for light map calculations. Same structure as above but with vec2s instead.
	};

	class PhysicsQuad {
		

	public:
		
	};

	class PhysicsPoint {
		

	public:
		
	};
}

#endif