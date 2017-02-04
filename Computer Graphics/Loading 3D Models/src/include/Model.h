#pragma once

#include <vector>
#include "PLYModel.h"

class Model
{
public:	
	PLYModel model3D;
	void render();
	void drawAxis();
	void drawBoundingBox();
	void rotate(float deg);
	void scale(float times);
	void translate(float x, float y);
};
