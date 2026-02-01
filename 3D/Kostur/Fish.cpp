#include "../Kostur/Header/Fish.h"
#include "../Kostur/Header/Bubble.h"
#include <GLFW/glfw3.h>

Fish::Fish() : x(0), y(0), w(0), h(0), speed(0), facingRight(true), texture(0), nextBubbleTime(0.0f) {}

void Fish::init(float x_, float y_, float w_, float h_, float speed_, bool facingRight_) {
	x = x_;
	y = y_; 
	w = w_;
	h = h_; 
	speed = speed_; 
	facingRight = facingRight_; 
	texture = 0; 
	nextBubbleTime = 0.0f;
}



void Fish::clampToBounds(int fbW, int fbH) {
	if (x < 0) {
		x = 10.0f;
	}
	if (x + w > fbW) {
		x = fbW - w;
	}
	if (y > fbH) { 
		y = fbH - h; 
	}
	if (y < fbH * 0.40f) {
		y = fbH * 0.40f;
	}
}


static float calculateBubbleX(const Fish& fish) {
	if (fish.facingRight) {
		return fish.x + fish.w;
	}
	else {
		return fish.x;
	}
}


static Bubble makeBubbleFromFish(const Fish& fish) {
	Bubble b;
	b.init(calculateBubbleX(fish), fish.y + fish.h / 2.0f, 30.0f, 7000.0f);
	return b;
}


void Fish::trySpawnBubbles(std::vector<Bubble>& bubbles, int& bubbleCount) {
	float now = (float)glfwGetTime();
	if (bubbleCount > 0 && now >= nextBubbleTime) {
		Bubble b = makeBubbleFromFish(*this);
		bubbles.push_back(b);
		bubbleCount--;
		nextBubbleTime = now + 0.3f; 
	}
}