#ifndef SFTE_TEXT_HPP
#define SFTE_TEXT_HPP

#include "core.hpp"

namespace sfte {
	class Text {
		sf::Texture* texture;
		sf::Vector2u charBounds;
		sf::VertexArray vertexArray;

	public:
		Text(sf::Texture* fontTexture, sf::Vector2u characterBounds);
		void print(sf::RenderTarget* renderTarget, std::string toPrint, sf::Vector2f position, sf::Vector2f fontSize, size_t maxColumns = 0, sf::Color color = sf::Color::White, bool shadowing = false, sf::Vector2f shadowOffset = sf::Vector2f(1.0f, 1.0f), float shadowFactor = 1.0f);
	};
}

#endif