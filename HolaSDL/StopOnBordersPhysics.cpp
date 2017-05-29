#include "StopOnBordersPhysics.h"
#include "Vector2D.h"

StopOnBordersPhysics::StopOnBordersPhysics(bool left, bool right, bool top,
		bool bottom) :
		left_(left), right_(right), top_(top), bottom_(bottom) {
}

StopOnBordersPhysics::~StopOnBordersPhysics() {
}

void StopOnBordersPhysics::init(GameObject* o) {
}


void StopOnBordersPhysics::update(GameObject* o) {

	Vector2D<int> nextPos = o->getPosition() + o->getDirection();
	
	// check if we exit from top/bottom
	if (top_ && nextPos.getY() <= 0) {
		nextPos.setY(0);
		o->setPosition(nextPos);
		o->setDirectionY(-1 * o->getDirection().getY());
	}

	if (bottom_
		&& nextPos.getY()
		>= o->getGame()->getWindowHeight() - o->getHeight()) {
		nextPos.setY(o->getGame()->getWindowHeight() - o->getHeight());
		o->setDirectionY(-1 * o->getDirection().getY());		
	}

	o->setPosition(nextPos);


	/*if (o->getPosition().getY() + o->getDirection().getY() < 0 && top_) {
		o->setDirectionY(0);
		o->setPositionY(0);
	}
	else if (o->getPosition().getY() + o->getDirection().getY() > 100 && bottom_) {
		o->setDirectionY(0);
		o->setPositionY(0);
	}*/
}
