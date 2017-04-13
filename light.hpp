#ifndef SFTE_LIGHT_HPP
#define SFTE_LIGHT_HPP

#include "world.hpp"
#include "containers.hpp"
#include "physics.hpp"

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
		CollisionProperties* collisionProperties;			// Pointer to CollisionProperties table for collision checking.
		std::vector< PhysicsProperty >* physicsProperties;	// Pointer to PhysicsProperty table for collision checking.
		sf::VertexArray va;									// Vertex array to iterate over lights
		sf::Vector2f viewSize;								// Possibly temporary. Size of window.

	public:
		void castLightRay(float x1, float y1, float x2, float y2, float leftB, float upB, float rightB, float downB, float bleed, std::vector < sf::Vector2f >* vec);
		void render(sf::Vector2f tlScreenPoint, sf::Vector2f brScreenPoint);
		void renderLight(sf::Vector2f position, float radius, float bleed);

		LightMap(World < worldTileIDType >* world, CollisionProperties* collisionProps, std::vector< PhysicsProperty >* physicsProps);
	};

	/* sfte::LightMap implementation.
	   Template for EVERY function in sfte::LightMap:

		template< class worldTileIDType, typename IDType > RETURNTYPE LightMap< worldTileIDType, IDType >::NAME() {
			;
		}
	*/
		template< class worldTileIDType, typename IDType > LightMap< worldTileIDType, IDType >::LightMap(World < worldTileIDType >* world, CollisionProperties* collisionProps, std::vector< PhysicsProperty >* physicsProps) :
		    va(sf::TrianglesFan),
		    viewSize(world->getRenderTarget()->getView().getSize())
		{
			targetWorld = world;
			collisionProperties = collisionProps;
			physicsProperties = physicsProps;

			lightmap.create(ceil(viewSize.x), ceil(viewSize.y));
		}

		template< class worldTileIDType, typename IDType > void LightMap< worldTileIDType, IDType >::castLightRay(float x1, float y1, float x2, float y2, float leftB, float upB, float rightB, float downB, float bleed, std::vector < sf::Vector2f >* vec) {
			// The DDA algorithm used here was taken from: http://lodev.org/cgtutor/raycasting.html
			// All credits regarding the DDA go to the author of the algorithm. Thank you whoever made it!

			// Check special cases:
			if((x1 == x2) && (y1 == y2)) {
				return; // Prevent crash by aborting. Point can't be equal to origin
			}
			else if(x1 == x2) { // Special case if line goes directly up or down (x aligned). Prevent crash of div by 0
				int x = floor(x1),
					y = floor(y1);

				if(y1 < y2) { // Goes down
					if(y2 > downB)
						y2 = downB;
					int endY = floor(y2); // No endX needed, x remains equal

					while(y <= endY) {
						IDType tileID = targetWorld->tile(sf::Vector3u(x, y, 0));
						if(physicsProperties->at(tileID).tangible) {
							unsigned char bitmask = targetWorld->getTileBitmask(sf::Vector3u(x, y, 0));
							size_t colID = physicsProperties->at(tileID).collisionID,
								   iMax = collisionProperties->edges[colID][bitmask].size();
							bool found = false;
							float lastDist;
							sf::Vector2f final;

							for(size_t i = 0; i < iMax; ++i) {
								if(!collisionProperties->edges[colID][bitmask][i].x) { // y aligned (- shape)
									if((x1 >= (collisionProperties->edges[colID][bitmask][i].s + x)) && (x1 <= (collisionProperties->edges[colID][bitmask][i].b + x))) {
										float thisDist = collisionProperties->edges[colID][bitmask][i].a + y - y1;
										if((!found) || (thisDist < lastDist)) {
											lastDist = thisDist;
											found = true;
											final.y = collisionProperties->edges[colID][bitmask][i].a + y;
										}
									}
								}
							}

							if(found) {
								final.x = x1;
								final.y += bleed;
								if(final.y > downB)
									final.y = downB;
								vec->push_back(final);
								return;
							}
						}

						++y;
					}
				}
				else { // Goes up
					if(y2 < upB)
						y2 = upB;
					int endY = floor(y2); // No endX needed, x remains equal

					while(y >= endY) {
						IDType tileID = targetWorld->tile(sf::Vector3u(x, y, 0));
						if(physicsProperties->at(tileID).tangible) {
							unsigned char bitmask = targetWorld->getTileBitmask(sf::Vector3u(x, y, 0));
							size_t colID = physicsProperties->at(tileID).collisionID,
								   iMax = collisionProperties->edges[colID][bitmask].size();
							bool found = false;
							float lastDist;
							sf::Vector2f final;

							for(size_t i = 0; i < iMax; ++i) {
								if(!collisionProperties->edges[colID][bitmask][i].x) { // y aligned (- shape)
									if((x1 >= (collisionProperties->edges[colID][bitmask][i].s + x)) && (x1 <= (collisionProperties->edges[colID][bitmask][i].b + x))) {
										float thisDist = y1 - collisionProperties->edges[colID][bitmask][i].a - y;
										if((!found) || (thisDist < lastDist)) {
											lastDist = thisDist;
											found = true;
											final.y = collisionProperties->edges[colID][bitmask][i].a + y;
										}
									}
								}
							}

							if(found) {
								final.x = x1;
								final.y -= bleed;
								if(final.y < upB)
									final.y = upB;
								vec->push_back(final);
								return;
							}
						}

						--y;
					}
				}
			}
			else if(y1 == y2) { // Special case if line goes directly left or right. Also prevents crash
				int x = floor(x1),
					y = floor(y1);

				if(x1 < x2) { // Goes right
					if(x2 > rightB)
						x2 = rightB;
					int endX = floor(x2); // No endY needed, y remains equal

					while(x <= endX) {
						IDType tileID = targetWorld->tile(sf::Vector3u(x, y, 0));
						if(physicsProperties->at(tileID).tangible) {
							unsigned char bitmask = targetWorld->getTileBitmask(sf::Vector3u(x, y, 0));
							size_t colID = physicsProperties->at(tileID).collisionID,
								   iMax = collisionProperties->edges[colID][bitmask].size();
							bool found = false;
							float lastDist;
							sf::Vector2f final;

							for(size_t i = 0; i < iMax; ++i) {
								if(collisionProperties->edges[colID][bitmask][i].x) { // x aligned (| shape)
									if((y1 >= (collisionProperties->edges[colID][bitmask][i].s + y)) && (y1 <= (collisionProperties->edges[colID][bitmask][i].b + y))) {
										float thisDist = collisionProperties->edges[colID][bitmask][i].a + x - x1;
										if((!found) || (thisDist < lastDist)) {
											lastDist = thisDist;
											found = true;
											final.x = collisionProperties->edges[colID][bitmask][i].a + x;
										}
									}
								}
							}

							if(found) {
								final.x += bleed;
								if(final.x > rightB)
									final.x = rightB;
								final.y = y1;
								vec->push_back(final);
								return;
							}
						}

						++x;
					}
				}
				else { // Goes left
					if(x2 < leftB)
						x2 = leftB;
					int endX = floor(x2); // No endY needed, y remains equal

					while(x >= endX) {
						IDType tileID = targetWorld->tile(sf::Vector3u(x, y, 0));
						if(physicsProperties->at(tileID).tangible) {
							unsigned char bitmask = targetWorld->getTileBitmask(sf::Vector3u(x, y, 0));
							size_t colID = physicsProperties->at(tileID).collisionID,
								   iMax = collisionProperties->edges[colID][bitmask].size();
							bool found = false;
							float lastDist;
							sf::Vector2f final;

							for(size_t i = 0; i < iMax; ++i) {
								if(collisionProperties->edges[colID][bitmask][i].x) { // x aligned (| shape)
									if((y1 >= (collisionProperties->edges[colID][bitmask][i].s + y)) && (y1 <= (collisionProperties->edges[colID][bitmask][i].b + y))) {
										float thisDist = x1 - collisionProperties->edges[colID][bitmask][i].a - x;
										if((!found) || (thisDist < lastDist)) {
											lastDist = thisDist;
											found = true;
											final.x = collisionProperties->edges[colID][bitmask][i].a + x;
										}
									}
								}
							}

							if(found) {
								final.x -= bleed;
								if(final.x < leftB)
									final.x = leftB;
								final.y = y1;
								vec->push_back(final);
								return;
							}
						}

						--x;
					}
				}
			}
			else { // Standard solution
				// Calculate gradient and y-intersect:
				float m = (y2 - y1) / (x2 - x1),
					  c = y2 - (m * x2);

				extendRayToBounds(x1, y1, x2, y2, m, c, leftB, upB, rightB, downB); // Extend ray to remove OOB checks

				int x = floor(x1),
					y = floor(y1),
					endX = floor(x2),
					endY = floor(y2);

				//length of ray from one x or y-side to next x or y-side
				float rayDirX = x2 - x1,
					  rayDirY = y2 - y1,
					  deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX)),
					  deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY)),
				//length of ray from current position to next x or y-side
					  sideDistX,
					  sideDistY;

				//what direction to step in x or y-direction (either +1 or -1)
				int stepX,
					stepY;

				//calculate step and initial sideDist
				if(rayDirX < 0) {
					stepX = -1;
					sideDistX = (x1 - x) * deltaDistX;
				}
				else {
					stepX = 1;
					sideDistX = (x + 1.0 - x1) * deltaDistX;
				}
				if(rayDirY < 0) {
					stepY = -1;
					sideDistY = (y1 - y) * deltaDistY;
				}
				else {
					stepY = 1;
					sideDistY = (y + 1.0 - y1) * deltaDistY;
				}

				do {
					IDType tileID = targetWorld->tile(sf::Vector3u(x, y, 0));
					if(physicsProperties->at(tileID).tangible) {
						unsigned char bitmask = targetWorld->getTileBitmask(sf::Vector3u(x, y, 0));
						size_t colID = physicsProperties->at(tileID).collisionID,
							   iMax = collisionProperties->edges[colID][bitmask].size();
						bool found = false;
						float lastDist;
						sf::Vector2f final;

						for(size_t i = 0; i < iMax; ++i) {
							if(collisionProperties->edges[colID][bitmask][i].x) { // x aligned (| shape)
								float thisX = collisionProperties->edges[colID][bitmask][i].a + x,
									  thisY = (m * thisX) + c;
								if((thisY >= (collisionProperties->edges[colID][bitmask][i].s + y)) && (thisY <= (collisionProperties->edges[colID][bitmask][i].b + y))) {
									float distX = x1 - thisX,
										  distY = y1 - thisY,
										  thisDist = std::sqrt((distX * distX) + (distY * distY));
									if((!found) || (thisDist < lastDist)) {
										lastDist = thisDist;
										found = true;
										final.x = thisX;
										final.y = thisY;
									}
								}
							}
							else { // y aligned (- shape)
								float thisY = collisionProperties->edges[colID][bitmask][i].a + y,
									  thisX = (thisY - c) / m;
								if((thisX >= (collisionProperties->edges[colID][bitmask][i].s + x)) && (thisX <= (collisionProperties->edges[colID][bitmask][i].b + x))) {
									float distX = x1 - thisX,
										  distY = y1 - thisY,
										  thisDist = std::sqrt((distX * distX) + (distY * distY));
									if((!found) || (thisDist < lastDist)) {
										lastDist = thisDist;
										found = true;
										final.x = thisX;
										final.y = thisY;
									}
								}
							}
						}

						if(found) {
							if(bleed != 0.0f)
								extendRayCircle(x1, y1, final.x, final.y, bleed, m, c, leftB, upB, rightB, downB);
							vec->push_back(final);
							return;
						}
					}

					if(sideDistX < sideDistY) {
						sideDistX += deltaDistX;
						x += stepX;
					}
					else {
						sideDistY += deltaDistY;
						y += stepY;
					}
				} while(((stepX == 1) ? (x <= endX) : (x >= endX)) && ((stepY == 1) ? (y <= endY) : (y >= endY))); // Correct and slower loop exit checking, but NOT glitchy, contrary to previous method
			}
			vec->push_back(sf::Vector2f(x2, y2));
		}

		template< class worldTileIDType, typename IDType > void LightMap< worldTileIDType, IDType >::renderLight(sf::Vector2f position, float radius, float bleed) {
	        sf::Vector2u tilesize(targetWorld->getTileSize());
	        sf::Vector2f worldbound(targetWorld->getTilemapSize().x - (0.5f / float(tilesize.x)), targetWorld->getTilemapSize().y - (0.5f / float(tilesize.y)));

	        float oX = position.x / tilesize.x,
	        	  oY = position.y / tilesize.y,
	        	  x1 = (position.x - radius) / tilesize.x,
	        	  y1 = (position.y - radius) / tilesize.y,
	        	  x2 = (position.x + radius) / tilesize.x,
	        	  y2 = (position.y + radius) / tilesize.y;

	        if((oX < 0) || (oX > worldbound.x) || (oY < 0) || (oY > worldbound.y)) {
	        	return;
	        }

			std::vector < sf::Vector2f > points; // Vector to store collision points for later ordering

	        if(x1 < 0) {
	        	x1 = 0;
	        }
	        else if(x1 > worldbound.x) {
	        	x1 = worldbound.x;
	        }
	        if(y1 < 0) {
	        	y1 = 0;
	        }
	        else if(y1 > worldbound.y) {
	        	y1 = worldbound.y;
	        }
	        if(x2 < 0) {
	        	x2 = 0;
	        }
	        else if(x2 > worldbound.x) {
	        	x2 = worldbound.x;
	        }
	        if(y2 < 0) {
	        	y2 = 0;
	        }
	        else if(y2 > worldbound.y) {
	        	y2 = worldbound.y;
	        }

			// Add screen corner points
			castLightRay(oX, oY, x1, y1, x1, y1, x2, y2, bleed, &points); // TL
			castLightRay(oX, oY, x2, y1, x1, y1, x2, y2, bleed, &points); // TR
			castLightRay(oX, oY, x2, y2, x1, y1, x2, y2, bleed, &points); // BR
			castLightRay(oX, oY, x1, y2, x1, y1, x2, y2, bleed, &points); // BL

	        for(size_t x = floor(x1); x <= floor(x2); ++x) {
	        	for(size_t y = floor(y1); y <= floor(y2); ++y) {
					IDType tileID = targetWorld->tile(sf::Vector3u(x, y, 0));
					if(!physicsProperties->at(tileID).tangible)
						continue;
					size_t id = physicsProperties->at(tileID).collisionID;
					unsigned char bitmask = targetWorld->getTileBitmask(sf::Vector3u(x, y, 0));
					for(size_t i = 0; i < collisionProperties->points[id][bitmask].size(); ++i)
						castLightRay(oX, oY, float(x) + collisionProperties->points[id][bitmask][i].x, float(y) + collisionProperties->points[id][bitmask][i].y, x1, y1, x2, y2, bleed, &points);
		        }
	        }

	        std::sort(points.begin(), points.end(), [&oX, &oY](sf::Vector2f a, sf::Vector2f b) { return atan2(a.y - oY, a.x - oX) < atan2(b.y - oY, b.x - oX); });

	        sf::Vector2f centre(targetWorld->getRenderTarget()->getView().getCenter()),
	        			 viewPos(centre.x - (viewSize.x * 0.5f), centre.y - (viewSize.y * 0.5f));

	        va.clear();
	        va.append(sf::Vertex(sf::Vector2f(position.x - viewPos.x, position.y - viewPos.y)));
	        for(size_t n = 0; n < points.size(); ++n)
	        	va.append(sf::Vertex(sf::Vector2f(points[n].x * tilesize.x - viewPos.x, points[n].y * tilesize.y - viewPos.y)));
	        va.append(sf::Vertex(sf::Vector2f(points[0].x * tilesize.x - viewPos.x, points[0].y * tilesize.y - viewPos.y)));

	        lightmap.clear(sf::Color(64, 64, 64));
	        lightmap.draw(va);
	        lightmap.display();

	        sf::Sprite lightmapSpr(lightmap.getTexture());
	        lightmapSpr.setPosition(viewPos);
	        targetWorld->getRenderTarget()->draw(lightmapSpr, sf::RenderStates(sf::BlendMultiply));
	    }
}

#endif