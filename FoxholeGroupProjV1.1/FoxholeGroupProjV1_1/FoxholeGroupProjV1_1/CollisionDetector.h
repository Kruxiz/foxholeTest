#ifndef COLLISIONDETECTOR_H
#define COLLISIONDETECTOR_H

#include "GameObject.h"

class CollisionDetector {
public:
	static bool detectCollision(GameObject objA, GameObject objB);
};

#endif
