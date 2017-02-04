#pragma once

#include "Point3d.h"

class Line										//A simple class for passing a line to OpenGL
{
public:
		
		Line()									//Constructor: Starts drawing the Line
		{  glBegin(GL_LINES); }
	   ~Line()
	    {}
		
void	addPoint(float x, float y, float z)		//Adds a new vertex to the Line
		{  glVertex3f(x,y,z); }

void	addPoint(Point3d p)		//Adds a new vertex to the Line
		{  glVertex3f(p.x,p.y,p.z); }

void    draw()									//Drawing function: ends the Line definition. This also draws the Line.
		{ glEnd(); }						  
};			 	


