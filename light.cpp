#include "light.hpp"

// sfte::PointLight implementation
sfte::PointLight::PointLight(sf::Vector2f lightPosition, float lightRadius, sf::Color lightColor) :
	position(lightPosition),
	radius(lightRadius),
	color(lightColor)
{ }