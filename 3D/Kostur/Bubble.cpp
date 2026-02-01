#include "../Kostur/Header/Bubble.h"

Bubble::Bubble() : x(0), y(0), size(0), speed(0), alive(false) {}


void Bubble::init(float x_, float y_, float size_, float speed_) {
	x = x_;
	y = y_;
	size = size_;
	speed = speed_;
	alive = true;
}


void Bubble::update(float deltaTime, float minY) {
	if (!alive) {
		return;
	}

	y -= speed * deltaTime;

	if (y < minY){
		alive = false;
	}

}