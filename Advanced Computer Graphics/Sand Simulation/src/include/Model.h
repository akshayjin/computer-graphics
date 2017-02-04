#pragma once

#include <vector>
#include "ParticleSystem.h"

class Model
{
public:	
	Model();
	ParticleSystem particleSystem;
	void render();
	void drawAxis();
	void drawBoundingBox();
	void rotate(float deg);
	void scale(float times);
	void translate(float x, float y);
};
