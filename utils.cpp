#include "utils.hpp"

namespace sfte {
	// sfte::Timer implementation
		void Timer::update() {
			deltaTime = clock.restart().asSeconds();
		}

		void Timer::clear() {
			deltaTime = 0.0f;
			clock.restart();
		}

		float Timer::delta() {
			return deltaTime;
		}

		float Timer::fps() {
			return 1 / deltaTime;
		}
	
	// sfte::CamController implementation
		sf::Vector2f CamController::getPosition() {
			sf::Vector2f center = currentRenderWindow->getView().getCenter(),
						 size = currentRenderWindow->getView().getSize();
			return sf::Vector2f(center.x - (size.x * 0.5f), center.y - (size.y * 0.5f));
		}

		void CamController::update(float deltaTime) {
			sf::Vector2i mousePos(sf::Mouse::getPosition(*currentRenderWindow));
			sf::Vector2f winSize(currentRenderWindow->getView().getSize()),
						 difference(mousePos.x - (winSize.x * 0.5f), mousePos.y - (winSize.y * 0.5f));
			float radialDistance = hypot(difference.x, difference.y) / circle.getRadius();

			if(radialDistance <= 1.0f) {
				float angle = std::atan2(difference.x, difference.y);
				sf::View currentView(currentRenderWindow->getView());
				currentView.move(sin(angle) * sin(M_PI * radialDistance) * deltaTime * multi, cos(angle) * sin(M_PI * radialDistance) * deltaTime * multi);
				currentRenderWindow->setView(currentView);
			}
		}

		void CamController::render() {
			sf::Vector2f center(currentRenderWindow->getView().getCenter());
			circle.setPosition(center.x - circle.getRadius(), center.y - circle.getRadius());
			currentRenderWindow->draw(circle);
		}

		CamController::CamController(sf::RenderWindow* renderWindow, float radius, float speed) :
			currentRenderWindow(renderWindow),
			circle(radius),
			multi(speed)
		{
			circle.setFillColor(sf::Color(255, 255, 255, 64));
			circle.setOutlineThickness(2);
			circle.setOutlineColor(sf::Color(255, 255, 255, 128));
		}

	// sfte::Console implementation
		void Console::clear() {
			str = "";
		}

		void Console::update() {
			needsUpdate = false;
			if(!str.empty()) {
				std::vector < size_t > lVec;
				for(size_t n = 0, l = 0; n < str.size(); ++n) {
					++l;
					if((str[n] == '\n') || (l == m_maxColumns)) {
						lVec.push_back(n);
						l = 0;
					}
					else if(n + 1 == str.size())
						lVec.push_back(n);
				}

				if(lVec.size() > m_maxLines)
					str = str.substr(lVec[lVec.size() - m_maxLines - 1] + 1);
			}
		}

		void Console::restoreCout() {
			if(oldCout != nullptr) {
    			std::cout.rdbuf(oldCout); // std::cout must be restored in the end of main or a segfault occurs.
    			oldCout = nullptr;
    		}
		}

		void Console::redirectCout() {
			restoreCout();
		    oldCout = std::cout.rdbuf(coutBuffer.rdbuf()); // Save old std::cout buffer pointer for later restore
		}

		Console& Console::operator<<(const std::string toInsert) {
			str += toInsert;
			needsUpdate = true;
			return *this;
		}

		Console& Console::operator<<(const char* toInsert) {
			str += toInsert;
			needsUpdate = true;
			return *this;
		}

		Console& Console::operator<<(const bool toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}

		Console& Console::operator<<(const char toInsert) {
			str += toInsert;
			needsUpdate = true;
			return *this;
		}

		Console& Console::operator<<(const unsigned char toInsert) {
			str += toInsert;
			needsUpdate = true;
			return *this;
		}

		Console& Console::operator<<(const short toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const unsigned short toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const int toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const unsigned int toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const long toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const unsigned long toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const long long toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const unsigned long long toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const float toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const double toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}
		
		Console& Console::operator<<(const long double toInsert) {
			str += std::to_string(toInsert);
			needsUpdate = true;
			return *this;
		}

		void Console::render(sf::Vector2f position) {
			// Update console with cout contents
	        if(coutBuffer.rdbuf()->in_avail() > 0) {
	            str += coutBuffer.str();
	            coutBuffer.str("");
	            coutBuffer.clear();
	            needsUpdate = true;
	        }

			if(needsUpdate)
				update();
			textRenderer.print(currentRenderTarget, str, position, m_fontSize, m_maxColumns, m_color, m_shadowing, m_shadowOffset, m_shadowFactor);
		}

		void Console::setRenderTarget(sf::RenderTarget* whereToDraw) {
			currentRenderTarget = whereToDraw;
		}

		Console::Console(sf::RenderTarget* whereToDraw, sf::Texture* fontTexture, sf::Vector2u characterBounds, sf::Vector2f fontSize, size_t maxLines, size_t maxColumns, sf::Color color, bool shadowing, sf::Vector2f shadowOffset, float shadowFactor) :
			currentRenderTarget(whereToDraw),
			textRenderer(fontTexture, characterBounds),
			m_fontSize(fontSize),
			m_maxLines(maxLines),
			m_maxColumns(maxColumns),
			m_color(color),
			m_shadowing(shadowing),
			m_shadowOffset(shadowOffset),
			m_shadowFactor(shadowFactor)
		{}

		Console::~Console() {
			restoreCout();
		}
}