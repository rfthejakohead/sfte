#ifndef SFTE_CORE_HPP
#define SFTE_CORE_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <math.h>
#include <time.h>
#include <algorithm>

namespace sf {
	typedef sf::Vector3< size_t > Vector3u;
}

namespace sfte {
	struct FloatColor { // The unit used by sfte to represent color. Can be converted to and from SFML colors and is to be used with OpenGL and maths.
		float r, // This is where the rgba color values of the struct are stored.
			  g, // It is done this way so that they can be accessed like the SFML
			  b, // sf::Color class, by doing (for example) FloatColor::r = 0.75f.
			  a;

		FloatColor operator*(const FloatColor& obj); //Operator for multiplication between FloatColors

		sf::Color getSfColor(); //Convert to SFML 8-bit color

		FloatColor(float red, float green, float blue, float alpha = 1.0f); //Contructor from floats
		FloatColor(sf::Color color); //Contructor from sf:Color
		FloatColor(); //Default contructor. Defaults to opaque black (0.0, 0.0, 0.0, 1.0 rgba)
	};
}

#endif