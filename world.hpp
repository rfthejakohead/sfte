#ifndef SFTE_WORLD_HPP
#define SFTE_WORLD_HPP

#include "core.hpp"

/*/////////////////////////////
		Space in SFTE
		 1_________
		z/		  /
	   0/________/|
	   0|0	x  1| |
		|		| |
	   y|		| |
		|		| /
	   1|_______|/

/////////////////////////////*/

namespace sfte {
	enum VisibilityMode {
		visibilityOpaque,
		visibilityTransparentEdges,
		visibilityTransparent
	};

	struct TileProperty {
		sf::Vector2f tcTL, // TexCoord top left.
					 tcBR; // TexCoord bottom right _DELTA_, NOT THE ACTUAL TEXCOORD! (so this is the difference between the BotRight and TopLeft texcoords!)
		bool render; // Is this tile invisible? false = invisible, true = visible
		VisibilityMode visibility; // Visibility mode (optimisations only)
		unsigned char connectiveID; // What the tile connects with in the bitmask. A special value of 0 indicates that it doesn't connect to anything.

		TileProperty(sf::Vector2f texCoordTopLeft, sf::Vector2f texCoordBottomRight, bool visible = true, VisibilityMode visibilityMode = visibilityTransparent, unsigned char connectsTo = 0);
	};

	template< typename tileIDType = size_t > class World {
		// Class private members. These include implementation exclusive functions and private variables.
		// Some members could be accessed directly, but it is more pretty to give them an access function.
		std::vector< TileProperty >* tileProperties;							// Pointer to tile properties table.
		sf::Vector3u tilemapSize,												// Size of the tilemap.
					 tilemapLimits;												// Same as above but with -1, for convineance
		sf::Vector2u tileSize;													// Size of each tile in pixels (for tile boundaries).
		sf::Texture* tilemapTexture;											// Pointer to the texture to be used for tilemap rendering.
		std::vector< sf::Color > layerColor;									// Color of tiles when in each layer.
		sf::RenderTarget* currentRenderTarget;									// Pointer to the current render target (where to render).
		std::vector< std::vector < std::vector < tileIDType > > > tilemap;		// Vector to store all of the tiles in the tilemap.
		std::vector< std::vector < char > > occludermap;						// Occluder map.
		std::vector< std::vector < std::vector < unsigned char > > > bitmask;	// Bitmask (for custom edges from texture atlas).
		sf::VertexArray tilemapVA;												// The vertex array for the tilemap (contains geometry).
																				// This is only kept for optimisation purposes.
		bool redraw = false;													// This is also only for optimisations. Indicates VA redraw.
		sf::Vector2f lastTlScreenPoint,											//                  ''                . Stores last screen points
					 lastBrScreenPoint;											// to compare to the new ones.

		// Occluder map related functions
		inline bool isOccluder(sf::Vector3u position);
		inline void updateOccluder(sf::Vector2u position);

		// Bitmask related functions
		inline void updateBitmask(sf::Vector3u position);
	public:
		// More occluder map related functions
		void genOccluderMap();

		// More bitmask related functions
		void genBitmask();

		// Member access
		inline void		  tile(sf::Vector3u position, tileIDType ID);
		inline tileIDType tile(sf::Vector3u position);
		inline TileProperty getTileProperties(sf::Vector3u position);
		inline unsigned char getTileBitmask(sf::Vector3u position);
		inline sf::Vector3u getTilemapSize();
		inline sf::Vector3u getTilemapLimits();
		inline sf::Vector2u getTileSize();

		// Rendering
		void setRenderTarget(sf::RenderTarget* newRenderTarget);
		sf::RenderTarget* getRenderTarget();
		void render(sf::Vector2f tlScreenPoint, sf::Vector2f brScreenPoint);

		// Constructor
		World(std::vector< TileProperty >* tilePropertiesPointer, sf::Vector3u mapSize, sf::Vector2u tileSizeInPixels, sf::Texture* tilemapTexturePointer, std::vector < sf::Color > layerColors, tileIDType defaultID = 0, sf::RenderTarget* whereToDraw = nullptr);
	};

	/* sfte::World implementation. Because sfte::World is a template class it has to be implemented in the header, which is very ugly.
	   Template for EVERY function in sfte::world:

		template< typename tileIDType > RETURNTYPE World< tileIDType >::NAME() {
			;
		}
	*/
		template< typename tileIDType > inline bool World< tileIDType >::isOccluder(sf::Vector3u position) {
			// TODO: Implement visibilityTransparentEdges when bitmask is done
			return tileProperties->at(tilemap[position.x][position.y][position.z]).render && (tileProperties->at(tilemap[position.x][position.y][position.z]).visibility == visibilityOpaque || (tileProperties->at(tilemap[position.x][position.y][position.z]).visibility == visibilityTransparentEdges && bitmask[position.x][position.y][position.z] == 0));
		}

		template< typename tileIDType > inline void World< tileIDType >::updateOccluder(sf::Vector2u position) {
			sf::Vector3u pos(position.x, position.y, 0);
			for(; pos.z < tilemapSize.z; ++pos.z) {
				if((pos.z + 1 == tilemapSize.z) || isOccluder(pos)) {
					occludermap[pos.x][pos.y] = pos.z;
					break;
				}
			}
		}

		template< typename tileIDType > inline void World< tileIDType >::updateBitmask(sf::Vector3u position) {
			unsigned char whatMask = 0;
            if(tileProperties->at(tilemap[position.x][position.y][position.z]).render && (tileProperties->at(tilemap[position.x][position.y][position.z]).connectiveID != 0)){
            	unsigned char thisID = tileProperties->at(tilemap[position.x][position.y][position.z]).connectiveID;
                if (position.x > 0) {
                    if(tileProperties->at(tilemap[position.x - 1][position.y][position.z]).connectiveID != thisID) // Left
                        whatMask += 8;
                }
                if (position.x < tilemapLimits.x) {
                    if(tileProperties->at(tilemap[position.x + 1][position.y][position.z]).connectiveID != thisID) // Right
                        whatMask += 2;
                }
                if (position.y > 0) {
                    if(tileProperties->at(tilemap[position.x][position.y - 1][position.z]).connectiveID != thisID) // Top
                        whatMask += 1;
                }
                if (position.y < tilemapLimits.y) {
                    if(tileProperties->at(tilemap[position.x][position.y + 1][position.z]).connectiveID != thisID) // Bottom
                        whatMask += 4;
                }
            }
            bitmask[position.x][position.y][position.z] = whatMask;
		}

		template< typename tileIDType > void World< tileIDType >::genBitmask() {
			sf::Vector3u pos(0, 0, 0);
			for(; pos.x < tilemapSize.x; ++pos.x) {
				for(; pos.y < tilemapSize.y; ++pos.y) {
					for(; pos.z < tilemapSize.z; ++pos.z) {
						updateBitmask(pos);
					}
					pos.z = 0;
				}
				pos.y = 0;
			}
		}

		template< typename tileIDType > void World< tileIDType >::genOccluderMap() {
			sf::Vector3u pos(0, 0, 0);
			for(; pos.x < tilemapSize.x; ++pos.x) {
				for(; pos.y < tilemapSize.y; ++pos.y) {
					for(; pos.z < tilemapSize.z; ++pos.z) {
						if((pos.z + 1 == tilemapSize.z) || isOccluder(pos)) {
							occludermap[pos.x][pos.y] = pos.z;
							break;
						}
					}
					pos.z = 0;
				}
				pos.y = 0;
			}
		}

		template< typename tileIDType > inline void World< tileIDType >::tile(sf::Vector3u position, tileIDType ID) {
			tilemap[position.x][position.y][position.z] = ID; // Set tile ID of requested position to specified value.
		}

		template< typename tileIDType > inline tileIDType World< tileIDType >::tile(sf::Vector3u position) {
			return tilemap[position.x][position.y][position.z]; // Return tile ID of requested position.
		}

		template< typename tileIDType > inline TileProperty World< tileIDType >::getTileProperties(sf::Vector3u position) {
			return tileProperties->at(tilemap[position.x][position.y][position.z]); // Return tile properties of requested position.
		}

		template< typename tileIDType > inline unsigned char World< tileIDType >::getTileBitmask(sf::Vector3u position) {
			return bitmask[position.x][position.y][position.z]; // Return tile bitmask of requested position.
		}

		template< typename tileIDType > inline sf::Vector3u World< tileIDType >::getTilemapSize() {
			return tilemapSize; // Return tilemap size.
		}

		template< typename tileIDType > inline sf::Vector3u World< tileIDType >::getTilemapLimits() {
			return tilemapLimits; // Return tilemap limits.
		}

		template< typename tileIDType > inline sf::Vector2u World< tileIDType >::getTileSize() {
			return tileSize; // Return tile size.
		}

		template< typename tileIDType > void World< tileIDType >::render(sf::Vector2f tlScreenPoint, sf::Vector2f brScreenPoint) {
			if((tlScreenPoint.x >= tilemapSize.x) || (tlScreenPoint.y >= tilemapSize.y) || (brScreenPoint.x < 0) || (brScreenPoint.y < 0)) // Abort if out of bounds
				return;
			// Fix the boundaries in case it is bigger than the tilemap size:
			if(brScreenPoint.x >= tilemapSize.x)
				brScreenPoint.x = tilemapSize.x - 1;
			if(brScreenPoint.y >= tilemapSize.y)
				brScreenPoint.y = tilemapSize.y - 1;
			if(tlScreenPoint.x < 0)
				tlScreenPoint.x = 0;
			if(tlScreenPoint.y < 0)
				tlScreenPoint.y = 0;
			if((tlScreenPoint != lastTlScreenPoint) || (brScreenPoint != lastBrScreenPoint) || redraw) {
				lastTlScreenPoint = tlScreenPoint;
				lastBrScreenPoint = brScreenPoint;
				redraw = false;

				// Calculate geometry data:
				tilemapVA.clear();
				for(size_t y = lastTlScreenPoint.y; y <= lastBrScreenPoint.y; ++y) {
					for(size_t x = lastTlScreenPoint.x; x <= lastBrScreenPoint.x; ++x) {
						for(char z = occludermap[x][y]; z >= 0; --z) {
							if(tileProperties->at(tilemap[x][y][z]).render) {
								float yOffset = tileProperties->at(tilemap[x][y][z]).tcBR.y * bitmask[x][y][z];
								// Top-left triangle of tile
								tilemapVA.append(sf::Vertex(sf::Vector2f((x * tileSize.x) + tileSize.x, (y * tileSize.y) 			 ), layerColor[z], sf::Vector2f(tileProperties->at(tilemap[x][y][z]).tcTL.x + tileProperties->at(tilemap[x][y][z]).tcBR.x, tileProperties->at(tilemap[x][y][z]).tcTL.y + 											   yOffset))); // TR
								tilemapVA.append(sf::Vertex(sf::Vector2f((x * tileSize.x)			  , (y * tileSize.y) 			 ), layerColor[z], sf::Vector2f(tileProperties->at(tilemap[x][y][z]).tcTL.x												 , tileProperties->at(tilemap[x][y][z]).tcTL.y + 											   yOffset))); // TL
								tilemapVA.append(sf::Vertex(sf::Vector2f((x * tileSize.x)			  , (y * tileSize.y) + tileSize.y), layerColor[z], sf::Vector2f(tileProperties->at(tilemap[x][y][z]).tcTL.x												 , tileProperties->at(tilemap[x][y][z]).tcTL.y + tileProperties->at(tilemap[x][y][z]).tcBR.y + yOffset))); // BL
								// Bottom-right triangle of tile
								tilemapVA.append(sf::Vertex(sf::Vector2f((x * tileSize.x) + tileSize.x, (y * tileSize.y) 			 ), layerColor[z], sf::Vector2f(tileProperties->at(tilemap[x][y][z]).tcTL.x + tileProperties->at(tilemap[x][y][z]).tcBR.x, tileProperties->at(tilemap[x][y][z]).tcTL.y + 											   yOffset))); // TR
								tilemapVA.append(sf::Vertex(sf::Vector2f((x * tileSize.x)			  , (y * tileSize.y) + tileSize.y), layerColor[z], sf::Vector2f(tileProperties->at(tilemap[x][y][z]).tcTL.x												 , tileProperties->at(tilemap[x][y][z]).tcTL.y + tileProperties->at(tilemap[x][y][z]).tcBR.y + yOffset))); // BL
								tilemapVA.append(sf::Vertex(sf::Vector2f((x * tileSize.x) + tileSize.x, (y * tileSize.y) + tileSize.y), layerColor[z], sf::Vector2f(tileProperties->at(tilemap[x][y][z]).tcTL.x + tileProperties->at(tilemap[x][y][z]).tcBR.x, tileProperties->at(tilemap[x][y][z]).tcTL.y + tileProperties->at(tilemap[x][y][z]).tcBR.y + yOffset))); // BR
							}
						}
					}
				}
			}
			
			currentRenderTarget->draw(tilemapVA, tilemapTexture); // Draw the vertex array
		}
		
		template< typename tileIDType > void World< tileIDType >::setRenderTarget(sf::RenderTarget* newRenderTarget) {
			currentRenderTarget = newRenderTarget; // Set render target variable.
		}
		
		template< typename tileIDType > sf::RenderTarget* World< tileIDType >::getRenderTarget() {
			return currentRenderTarget; // Return render target variable.
		}

		template< typename tileIDType > World< tileIDType >::World(std::vector< TileProperty >* tilePropertiesPointer, sf::Vector3u mapSize, sf::Vector2u tileSizeInPixels, sf::Texture* tilemapTexturePointer, std::vector < sf::Color > layerColors, tileIDType defaultID, sf::RenderTarget* whereToDraw) :
			tileProperties(tilePropertiesPointer),
			tilemapSize(mapSize),
			tilemapLimits(mapSize.x - 1, mapSize.y - 1, mapSize.z - 1),
			tileSize(tileSizeInPixels),
			tilemapTexture(tilemapTexturePointer),
			layerColor(layerColors),
			currentRenderTarget(whereToDraw),
			tilemap(mapSize.x, std::vector< std::vector< tileIDType > >(mapSize.y, std::vector< tileIDType >(mapSize.z, defaultID))),
			occludermap(mapSize.x, std::vector < char >(mapSize.y)),
			bitmask(mapSize.x, std::vector< std::vector< unsigned char > >(mapSize.y, std::vector< unsigned char >(mapSize.z, 0))),
			tilemapVA(sf::PrimitiveType::Triangles)
		{}
}

#endif