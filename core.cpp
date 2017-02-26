#include "core.hpp"
#define _USE_MATH_DEFINES

namespace sfte {
	// sfte::FloatColor implementation
		FloatColor FloatColor::operator*(const FloatColor& obj) {
			return FloatColor(r * obj.r, g * obj.g, b * obj.b, a * obj.a);
		}

		sf::Color FloatColor::getSfColor() {
			return sf::Color(r * 255, g * 255, b * 255, a * 255);
		}

		FloatColor::FloatColor(float red, float green, float blue, float alpha) :
			r(red),
			g(green),
			b(blue),
			a(alpha)
		{}

		FloatColor::FloatColor(sf::Color color) :
			r(color.r / 255),
			g(color.g / 255),
			b(color.b / 255),
			a(color.a / 255)
		{}

		FloatColor::FloatColor() :
			r(0.0f),
			g(0.0f),
			b(0.0f),
			a(1.0f)
		{}
}