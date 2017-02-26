#include "text.hpp"

sfte::Text::Text(sf::Texture* fontTexture, sf::Vector2u characterBounds) :
    texture(fontTexture),
    charBounds(characterBounds),
    vertexArray(sf::PrimitiveType::Quads)
{}

void sfte::Text::print(sf::RenderTarget* renderTarget, std::string toPrint, sf::Vector2f position, sf::Vector2f fontSize, size_t maxColumns, sf::Color color, bool shadowing, sf::Vector2f shadowOffset, float shadowFactor) {
    if(!toPrint.empty()) {
        vertexArray.resize(toPrint.size() * (shadowing ? 8 : 4));
        size_t n = 0;
        for(size_t c = 0, row = 0, col = 0; c < toPrint.size(); ++c) {
            switch(toPrint[c]) {
            case '\n':
                ++row;
                col = 0;
                break;
            default:
                if(shadowing) {
                    vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x) + shadowOffset.x, position.y + (row * fontSize.y) + shadowOffset.y);
                    vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x, 0);
                    vertexArray[n++].color     = sf::Color(0, 0, 0, color.a * shadowFactor);
                    vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x) + fontSize.x + shadowOffset.x, position.y + (row * fontSize.y) + shadowOffset.y);
                    vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x + charBounds.x, 0);
                    vertexArray[n++].color     = sf::Color(0, 0, 0, color.a * shadowFactor);
                    vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x) + fontSize.x + shadowOffset.x, position.y + (row * fontSize.y) + fontSize.y + shadowOffset.y);
                    vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x + charBounds.x, charBounds.y);
                    vertexArray[n++].color     = sf::Color(0, 0, 0, color.a * shadowFactor);
                    vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x) + shadowOffset.x, position.y + (row * fontSize.y) + fontSize.y + shadowOffset.y);
                    vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x, charBounds.y);
                    vertexArray[n++].color     = sf::Color(0, 0, 0, color.a * shadowFactor);
                }
                vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x), position.y + (row * fontSize.y));
                vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x, 0);
                vertexArray[n++].color     = color;
                vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x) + fontSize.x, position.y + (row * fontSize.y));
                vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x + charBounds.x, 0);
                vertexArray[n++].color     = color;
                vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x) + fontSize.x, position.y + (row * fontSize.y) + fontSize.y);
                vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x + charBounds.x, charBounds.y);
                vertexArray[n++].color     = color;
                vertexArray[n  ].position  = sf::Vector2f(position.x + (col * fontSize.x), position.y + (row * fontSize.y) + fontSize.y);
                vertexArray[n  ].texCoords = sf::Vector2f(toPrint[c] * charBounds.x, charBounds.y);
                vertexArray[n++].color     = color;
                if((maxColumns != 0) && (col >= maxColumns - 1)) {
                    col = 0;
                    ++row;
                }
                else
                    ++col;
                break;
            }
        }
        vertexArray.resize(n);
        renderTarget->draw(vertexArray, texture);
    }
}