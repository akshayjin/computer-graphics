#pragma once

#include <vector>
#include "Point2D.h"
#include "Line.h"


class Model
{
	Line line_bezier;
	Line line_lagrange;
	std::vector <Point2D> points;
public:	
	void render();
	void drawAxis();
	void rotate(float deg);
	void scale(float times);
	void translate(float x, float y);
	void addPoint(Point2D point);
	void deletePoint(Point2D point);
	Model():line_bezier(1),line_lagrange(0){}
};
