#pragma once
#include <GL/glew.h>
#include <vector>



struct Bubble;


struct Fish {
	float x, y;
	float w, h;
	float speed;
	bool facingRight;
	GLuint texture;
	float nextBubbleTime;


	Fish();
	void init(float x_, float y_, float w_, float h_, float speed_, bool facingRight_);
	void clampToBounds(int fbW, int fbH);
	void trySpawnBubbles(std::vector<Bubble>& bubbles, int& bubbleCount);
};