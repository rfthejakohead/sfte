#ifndef SFTE_LIGHT_HPP
#define SFTE_LIGHT_HPP

#include "world.hpp"
#include "containers.hpp"

namespace sfte {
	struct PointLight {
		sf::Vector2f position;
		float radius;
		sf::Color color;

		PointLight(sf::Vector2f lightPosition, float lightRadius, sf::Color lightColor = sf::Color::White);
	};

	template < class worldTileIDType, typename IDType = size_t > class LightMap {
		World < worldTileIDType >* targetWorld;				// World with all the occluders.
		PointChunkMap < PointLight, IDType > lightChunks;	// PointLight chunks for storing them.
		sf::RenderTexture lightmap;							// TO REPLACE WITH ObjectGrid < sf::RenderTexture > LATER ON!
		inline bool pointCollides(int x, int y);
	public:
		sf::Vector2u castRay(int x1, int y1, int x2, int y2, bool keepGoing = false);
		void render(sf::Vector2f tlScreenPoint, sf::Vector2f brScreenPoint);

		LightMap(World < worldTileIDType >* world);
	};

	/* sfte::LightMap implementation.
	   Template for EVERY function in sfte::LightMap:

		template< class worldTileIDType, typename IDType > RETURNTYPE LightMap< worldTileIDType, IDType >::NAME() {
			;
		}
	*/
		template< class worldTileIDType, typename IDType > LightMap< worldTileIDType, IDType >::LightMap(World < worldTileIDType >* world) {
			targetWorld = world;
		}

		template< class worldTileIDType, typename IDType > inline bool LightMap< worldTileIDType, IDType >::pointCollides(int x, int y) {
			sf::Vector3u worldsize(targetWorld->getTilemapSize());
			sf::Vector2u tilesize(targetWorld->getTileSize());
			if((x >= 0) && (y >= 0) && (x / tilesize.x < worldsize.x) && (y / tilesize.y < worldsize.y)) {
				if(targetWorld->getTileProperties(sf::Vector3u(x / tilesize.x, y / tilesize.y, 0)).render)
					return true;
			}
			return false;
		}

		template< class worldTileIDType, typename IDType > sf::Vector2u LightMap< worldTileIDType, IDType >::castRay(int x1, int y1, int x2, int y2, bool keepGoing) {
			// Note: I DID NOT MAKE THIS! I took this algorithm from http://eugen.dedu.free.fr/projects/bresenham/, so thank you Eugen Dedu!
			// The only thing I did were the collision checks and the code beautifying.

			if(pointCollides(x1, y1))
				return sf::Vector2u(x1, y1);

			int xstep,
				ystep,
				error,
				x = x1,
				y = y1,
				dx = x2 - x1,
				dy = y2 - y1;

			if(dx < 0) {
				xstep = -1;
				dx = -dx;
			}
			else
				xstep = 1;
			if(dy < 0) {
				ystep = -1;
				dy = -dy;
			}
			else
				ystep = 1;

			int ddx = 2 * dx,
				ddy = 2 * dy;
			if(ddx >= ddy) {
				int errorprev = error = dx;
				for(int i = 0; i < dx; i++) {
					x += xstep;
					error += ddy;
					if(error > ddx) {
						y += ystep;
						error -= ddx;
						if(error + errorprev < ddx) { // Only vertical square
							if(pointCollides(x, y-ystep))
								return sf::Vector2u(x, y);
						}
						else if(error + errorprev > ddx) { // Only horizontal square
							if(pointCollides(x-xstep, y))
								return sf::Vector2u(x, y);
						}
						else if(pointCollides(x, y-ystep) && pointCollides(x-xstep, y)) // Both squares. Only count as collision if both of the squares collide.
							return sf::Vector2u(x, y);
					}
					if(pointCollides(x, y))
						return sf::Vector2u(x, y);
					errorprev = error;
				}
			}
			else {
				int errorprev = error = dy;
				for(int i = 0; i < dy; i++) {
					y += ystep;
					error += ddx;
					if (error > ddy){
						x += xstep;
						error -= ddy;
						if(error + errorprev < ddy) { // Only horizontal square
							if(pointCollides(x-xstep, y))
								return sf::Vector2u(x, y);
						}
						else if(error + errorprev > ddy) { // Only vertical square
							if(pointCollides(x, y-ystep))
								return sf::Vector2u(x, y);
						}
						else if(pointCollides(x, y-ystep) && pointCollides(x-xstep, y)) // Both squares. Only count as collision if both of the squares collide.
							return sf::Vector2u(x, y);
					}
					if(pointCollides(x, y))
						return sf::Vector2u(x, y);
					errorprev = error;
				}
			}
			return sf::Vector2u(x, y);
		}
}

#endif