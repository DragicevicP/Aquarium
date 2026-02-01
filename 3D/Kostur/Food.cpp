#include "../Kostur/Header/Food.h"
#include "../Kostur/Header/Fish.h"

Food::Food() : x(0), y(0), size(0), alive(false), speed(0) {}


void Food::init(float x_) {
	x = x_;
	y = 0.0f;
	size = 40.0f;
	alive = true;
	speed = 2000.0f;
}


void Food::update(float deltaTime, float sandTopY) {
	if (!alive) return;
	y += speed * deltaTime;
	if (y + size >= sandTopY) {
		y = sandTopY - size;
	}
}


bool Food::checkCollisionWithFish(Fish& fish) {
	if (!alive) return false;
	if (x + size > fish.x && x < fish.x + fish.w &&
		y + size > fish.y && y < fish.y + fish.h) {
		alive = false;
		fish.h += 0.01f; //fish.h += 0.5f;  
		return true;
	}
	return false;
}