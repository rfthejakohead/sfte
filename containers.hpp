#ifndef SFTE_CONTAINERS_HPP
#define SFTE_CONTAINERS_HPP

#include <stdexcept>
#include <deque>
#include "core.hpp"

namespace sfte {
	template < class objectType, typename IDType = size_t > class Sponge { // A container for random access which fills holes that are created on object removal
		std::vector < objectType > vec;
		std::deque < IDType > removed;
	public:

		objectType& operator[](IDType i);
		IDType add(objectType val);
		void del(IDType i);
	};

	/*	sfte::Sponge implementation. Has to be in the header for the same reason in world.hpp
		Implementation template:

		template< class objectType, typename IDType > RETURNTYPE Sponge< objectType, IDType >::NAME() {
			;
		}
	*/
		template< class objectType, typename IDType > objectType& Sponge< objectType, IDType >::operator[](IDType i) {
			for(IDType n : removed) {
				if(n == i)
					throw std::runtime_error("Cannot access a Sponge's hole (deleted member)! i = " + std::to_string(i));
			}

			return vec[i];
		}

		template< class objectType, typename IDType > IDType Sponge< objectType, IDType >::add(objectType val) {
			if(removed.size() > 0) {
				IDType i = removed.front();
				removed.pop_front();
				vec[i] = val;
				return i;
			}
			else {
				vec.push_back(val);
				return vec.size() - 1;
			}
		}

		template< class objectType, typename IDType > void Sponge< objectType, IDType >::del(IDType i) {
			if(i < 0)
				throw std::invalid_argument("Iterator i is negative in Sponge::del! i = " + i);

			if(i >= vec.size())
				throw std::range_error("Attempt to remove non-existent member of Sponge! i = " + std::to_string(i) + ";vec.size() = " + std::to_string(vec.size()));

			for(IDType n = 0; n < removed.size(); ++n) {
				if(removed[n] == i)
					throw std::runtime_error("Cannot access a Sponge's hole (deleted member)! i = " + std::to_string(i));
			}

			removed.push_back(i);
		}

	template < class objectType, typename IDType = size_t > class PointChunkMap { // A container for holding objects of point size in chunks of defined size
		std::vector < std::vector < std::vector < objectType* > > > chunks;	// The chunks of the chunkmap. Only stores the pointers to the objects for easy movement.
		Sponge < objectType, IDType > objectStack;							// Where the objects are actually held.
		sf::Vector2f chunkSize,
					 mapSize;
	public:
		
		void move(size_t objectID, sf::Vector2f position);
		size_t add(objectType object, sf::Vector2f position);
		void remove(size_t objectID);
	};
}

#endif