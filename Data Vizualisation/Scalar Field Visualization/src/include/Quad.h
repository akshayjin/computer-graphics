#pragma once



class Quad										//A simple class for passing a quad polygon to OpenGL
{
public:
		
		Quad()									//Constructor: Starts drawing the quad
		{  glBegin(GL_QUADS); }
	   ~Quad()
	    {}
		
void	addPoint(float x, float y, float z)		//Adds a new vertex to the quad
		{  glVertex3f(x,y,z); }

void	addNormal(float* n)						//Adds a new normal to the quad
		{  glNormal3f(n[0],n[1],n[2]); }
			
void    draw()									//Drawing function: ends the quad definition. This also draws the quad.
		{ glEnd(); }						  
};			 	




