#include "world.hpp"

namespace sfte {
	// sfte::TileProperty implementation
		TileProperty::TileProperty(sf::Vector2f texCoordTopLeft, sf::Vector2f texCoordSize, bool visible, VisibilityMode visibilityMode, unsigned char connectsTo) :
			tcTL(texCoordTopLeft),
			tcBR(texCoordSize),
			render(visible),
			visibility(visibilityMode),
			connectiveID(connectsTo)
		{}
}