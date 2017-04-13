#ifndef SFTE_MATH_HPP
#define SFTE_MATH_HPP

#include "core.hpp"

namespace sfte {
	void extendRayToBounds(float x1, float y1, float &x2, float &y2, float m, float c, float leftB, float upB, float rightB, float downB);
	void extendRaySquare(float x1, float y1, float &x2, float &y2, float a, float m, float c, float leftB, float upB, float rightB, float downB);
	void extendRayCircle(float x1, float y1, float &x2, float &y2, float a, float m, float c, float leftB, float upB, float rightB, float downB);
}

#endif