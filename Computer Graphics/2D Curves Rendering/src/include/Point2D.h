#pragma once

#include <GL/glut.h>	
#include <math.h>

class Point2D
{
public:
	float x;
	float y;
	Point2D(float xx,float yy) : x(xx), y(yy) {}
	void render()
	{
		glBegin( GL_POINTS );
			glColor3f( 1,0,0 );
			glVertex2f( x, y );
		glEnd();
	}
	
	bool operator< (const Point2D &other) const {
        return x < other.x;
    }
    
    float	dist(const Point2D &p) const { return sqrtf( (x-p.x)*(x-p.x) + (y-p.y)*(y-p.y) ); }
};
