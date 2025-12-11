#pragma once
struct Fish; 

struct Food {
	float x, y;
	float size;
	bool alive;
	float speed;


	Food();
	void init(float x_);
	void update(float deltaTime, float sandTopY);
	bool checkCollisionWithFish(Fish& fish);
};