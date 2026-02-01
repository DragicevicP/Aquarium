#pragma once
#include <GL/glew.h>


struct Bubble {
	float x, y;
	float size;
	float speed;
	bool alive;


	Bubble();
	void init(float x_, float y_, float size_, float speed_);
	void update(float deltaTime, float minY);
};