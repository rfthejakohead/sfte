#ifndef SFTE_UTILS_HPP
#define SFTE_UTILS_HPP

#include <sstream>
#include <fstream>
#include "text.hpp"

namespace sfte {
	class Timer {
		float deltaTime = 0.0f;
		sf::Clock clock;

	public:
		void update(); // Update deltaTime and restart clock. Always called on the end of a Frame.
		void clear(); // Clear deltaTime and restart clock. This shouldn't be called on every frame,
					  // because the previous deltaTime is erased, therefore it should only be called in the start of the first frame in the game.
		float delta(); // Get time elapsed in previous frame (delta time).
		float fps(); // Get FPS of previous frame (1 / delta time). Just a convenience function, since "1 / Timer::delta()" could be used instead.
	};
	
	struct CamController {
		sf::RenderWindow* currentRenderWindow;
		sf::CircleShape circle;
		float multi;

		sf::Vector2f getPosition();
		void update(float deltaTime);
		void render();

		CamController(sf::RenderWindow* renderWindow, float radius, float speed);
	};

	class Console {
		sf::RenderTarget* currentRenderTarget;
		Text textRenderer;
		bool needsUpdate = false;
		std::stringstream coutBuffer;
		std::streambuf* oldCout = nullptr;

		sf::Vector2f m_fontSize;
		size_t m_maxLines,
			   m_maxColumns;
		sf::Color m_color;
		bool m_shadowing;
		sf::Vector2f m_shadowOffset;
		float m_shadowFactor;
	public:
		std::string str;

		void clear();
		void update();
		void restoreCout();
		void redirectCout();
		Console& operator<<(const std::string toInsert);
		Console& operator<<(const char* toInsert);
		Console& operator<<(const bool toInsert);
		Console& operator<<(const char toInsert);
		Console& operator<<(const unsigned char toInsert);
		Console& operator<<(const short toInsert);
		Console& operator<<(const unsigned short toInsert);
		Console& operator<<(const int toInsert);
		Console& operator<<(const unsigned int toInsert);
		Console& operator<<(const long toInsert);
		Console& operator<<(const unsigned long toInsert);
		Console& operator<<(const long long toInsert);
		Console& operator<<(const unsigned long long toInsert);
		Console& operator<<(const float toInsert);
		Console& operator<<(const double toInsert);
		Console& operator<<(const long double toInsert);
		void render(sf::Vector2f position = sf::Vector2f(0.0f, 0.0f));
		void setRenderTarget(sf::RenderTarget* whereToDraw);

		Console(sf::RenderTarget* whereToDraw, sf::Texture* fontTexture, sf::Vector2u characterBounds, sf::Vector2f fontSize, size_t maxLines = 0, size_t maxColumns = 0, sf::Color color = sf::Color::White, bool shadowing = false, sf::Vector2f shadowOffset = sf::Vector2f(1.0f, 1.0f), float shadowFactor = 1.0f);
		~Console();
	};

	std::vector < std::vector < std::vector < sf::FloatRect > > > loadBoxesFromFile(std::string path); // Format loaders. Returns content, exception thrown for failure.
    std::vector < std::vector < std::vector < sf::Vector2f > > > loadCornersFromFile(std::string path);
}

#endif