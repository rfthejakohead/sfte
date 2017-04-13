#include "math.hpp"

void sfte::extendRayToBounds(float x1, float y1, float &x2, float &y2, float m, float c, float leftB, float upB, float rightB, float downB) {
	/*
	Extend ray to bounds to remove OOB checks and make points actually work like light rays (don't stop midair).
	This uses an algorithm made by me. The steps:
	1 - Get A:
		- Get X from upB or downB, depending on direction (if (m > 0) or (deltaY < 0) use up, else, use down)
		- Get Y from this X using x = (y - c) / m
	2 - Get B:
		- Get Y from leftB or rightB, depending on direction (if (deltaX < 0) use left, else, use right)
		- Get X from this Y using y = mx + c
	3 - Get distance from O (centre) of each point (A and B)
	4 - Use the point which is closest to O
	*/

	// Step 1 (get A):
	x2 = (y2 > y1) ? ((downB - c) / m) : ((upB - c) / m);
	y2 = (y2 > y1) ? downB : upB;
	// Step 2 (get B):
	float y3 = (x2 > x1) ? ((m * rightB) + c) : ((m * leftB) + c),
		  x3 = (x2 > x1) ? rightB : leftB,
	// Step 3 & 4 (get distances from O of each point and use point closest to O):
		  dx2 = x1 - x2,
		  dy2 = y1 - y2,
		  dx3 = x1 - x3,
		  dy3 = y1 - y3;
	if(sqrt((dx3 * dx3) + (dy3 * dy3)) < sqrt((dx2 * dx2) + (dy2 * dy2))) {
		x2 = x3;
		y2 = y3;
	}
}

void sfte::extendRaySquare(float x1, float y1, float &x2, float &y2, float a, float m, float c, float leftB, float upB, float rightB, float downB) {
	/*
	Extend ray by specified amount while keeping it in bounds and only extending axis farthest to origin (x1 and y1).
	Uses the previous algorithm to keep it in bounds.
	*/

	// Calculate distances
	float distX = (x1 > x2) ? (x1 - x2) : (x2 - x1),
		  distY = (y1 > y2) ? (y1 - y2) : (y2 - y1);

	bool snapToBounds = false;

	if(distX == distY) { // Special case to avoid expensive multiplication
		if(x1 > x2) {	// Left
			x2 -= a;
			if(x2 < leftB)
				snapToBounds = true;
		}
		else {			// Right
			x2 += a;
			if(x2 > rightB)
				snapToBounds = true;
		}

		if(y1 > y2) {	// Up
			y2 -= a;
			if(y2 < upB)
				snapToBounds = true;
		}
		else {			// Down
			y2 += a;
			if(y2 > downB)
				snapToBounds = true;
		}
	}
	else if(distX > distY) { // Increase X, since the axis is farthest to origin
		if(x1 > x2) {	// Left
			x2 -= a;
			if(x2 < leftB)
				snapToBounds = true;
		}
		else {			// Right
			x2 += a;
			if(x2 > rightB)
				snapToBounds = true;
		}

		// y = mx + c
		y2 = (m * x2) + c;

		if((y1 > y2) && (y2 < upB))			// Up
			snapToBounds = true;
		else if((y1 < y2) && (y2 > downB))	// Down
			snapToBounds = true;
	}
	else { // Increase Y, since the axis is farthest to origin
		if(y1 > y2) {	// Up
			y2 -= a;
			if(y2 < upB)
				snapToBounds = true;
		}
		else {			// Down
			y2 += a;
			if(y2 > downB)
				snapToBounds = true;
		}

		// x = (y - c) / m
		x2 = (y2 - c) / m;

		if((x1 > x2) && (x2 < leftB))		// Left
			snapToBounds = true;
		else if((x1 < x2) && (x2 > rightB))	// Right
			snapToBounds = true;
	}

	if(snapToBounds)
		extendRayToBounds(x1, y1, x2, y2, m, c, leftB, upB, rightB, downB);
}

void sfte::extendRayCircle(float x1, float y1, float &x2, float &y2, float a, float m, float c, float leftB, float upB, float rightB, float downB) {
	/*
	Extend ray by specified amount while keeping it in bounds. Different from *Square, since it extends hyp of deltas.
	Uses the extendRayToBounds algorithm to keep it in bounds.

	Steps:
	1 - Get triangle data off of deltas (get hyp with pythagoras and get angle with TOA)
	2 - Increment hyp
	3 - Calculate opposite (dY) using sine rule:
		.
		|\
		| \	hyp -> known
		|  \
		|___\
			  angles -> known (90 degrees and other is found in step 1 (i'll call it t because why not))

		  hyp 	   opp
		------- = ------
		sin(90)	  sin(t)

		opp     = hyp * sin(t)

	4 - Get adjacent using pythagoras:

		hyp^2 = adj^2 + opp^2
		adj   = sqrt(hyp^2 - opp^2)

	5 - Get the final x2 and y2
	6 - Snap to bounds if neccessary
	*/

	// Step 1
	float dX = (x1 > x2) ? (x1 - x2) : (x2 - x1),	// Adjacent
		  dY = (y1 > y2) ? (y1 - y2) : (y2 - y1),	// Opposite
		  hyp = sqrt((dX * dX) + (dY * dY)),		// Hypotenuse
		  angle = atan(dY / dX);					// Angle between hyp and adj

	// Step 2
	hyp += a;

	// Step 3
	dY = hyp * sin(angle);

	// Step 4
	dX = sqrt((hyp * hyp) - (dY * dY));

	// Step 5 & 6
	bool snapToBounds = false;

	if(x2 < x1) {	// Left
		x2 = x1 - dX;
		if(x2 < leftB)
			snapToBounds = true;
	}
	else {			// Right
		x2 = x1 + dX;
		if(x2 > rightB)
			snapToBounds = true;
	}

	if(y2 < y1) {	// Up
		y2 = y1 - dY;
		if(y2 < upB)
			snapToBounds = true;
	}
	else {			// Down
		y2 = y1 + dY;
		if(y2 > downB)
			snapToBounds = true;
	}
	
	if(snapToBounds)
		extendRayToBounds(x1, y1, x2, y2, m, c, leftB, upB, rightB, downB);
}