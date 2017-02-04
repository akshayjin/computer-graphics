#include "include/GlyphRenderer.h"
#include "include/ScalarAttributes.h"
#include "include/VectorAttributes.h"
#include "include/Grid.h"
#include <GL/glut.h>									//GLUT library
#include <algorithm>
#include <math.h>
#include <iostream>


using namespace std;


static float direction2angle(const Point3d& d)			//Converts a 2D vector into an orientation (angle).
{														//The angle is in the [0,360] degrees interval
	Point3d x = d; x.normalize();

	float cosa = x.x;
	float sina = x.y;
	
	float a;
	if (sina>=0)
		a = acos(cosa);
	else
		a = 2*M_PI - acos(cosa);
	
	return 180*a/M_PI;
}

void GlyphRenderer::drawCones(Grid& g)					//Draw 'g' by showing cone-glyphs, scaled and rotated to reflect the
{														//vector magnitude
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);

	VectorAttributes& vectors = g.pointVectors();		//     Get vector range, to normalize vector lengths next in [0,1]
	float vector_min = vectors.getMin();
	float vector_max = vectors.getMax();
	
	glMatrixMode(GL_MODELVIEW);
	for(int i=0;i<g.numPoints();++i)						
	{
		float v[2];
		g.getPoint(i,v);								//2.1. Get coordinates of i-th vertex	
		
		Point3d vec = g.pointVectors().getC0Vector(i); 		
		
		if(vec.x==0&&vec.y==0){
			glColor3f(1.0, 0.0, 0.0); 
			float u[2];
			g.getPoint(i+1,u);
			float l = abs(u[0]-v[0]);
			glBegin(GL_POLYGON); 
				glColor3f(1.0, 0.0, 0.0); 
				glVertex3f(v[0], v[1], 0.0); 
				glVertex3f(v[0]+l, v[1], 0.0); 
				glVertex3f(v[0]+l, v[1]+l, 0.0); 
				glVertex3f(v[0], v[1]+l, 0.0); 
			glEnd(); 
		}
		else{
			float length = (vec.norm()-vector_min)/(vector_max-vector_min);

			float r,g,b;									//2.2.  Get vector-length value at i-th vertex and normalize it to [0,1]
			mapToColor(length,r,g,b);						//2.3.  Map normalized length to a color 
			if(length==0)glColor3f(1,1,1);
			else glColor3f(r,g,b);								//2.4.  Pass color to OpenGL

			float scale = (scaling_mode==SCALE_NORMALIZE)? 1 : length; 
			float radius = scale*scaling_factor;			//2.5.  Compute the glyph's size, based on the vector length and scaling mode
			float angle = direction2angle(vec);				//2.6.  Convert the vector into an orientation (angle in [0,360] degrees)

			glPushMatrix();									//2.7.  Translate and rotate the canonical cone so as to be centered at the
			glTranslatef(v[0],v[1],0);						//      current vertex, and aligned with the current vector
			glRotatef(90-angle,0,0,-1);
			glRotatef(-90,1,0,0);
			glutSolidCone(radius/4,radius,30,30);			//2.8.  Draw the cone
			glPopMatrix();
		}
	}

	glDisable(GL_LIGHTING);
}

void GlyphRenderer::draw(Grid& g)						//Draw grid 'g' depending on drawing style attribute-value
{
	glClearColor(0,0,0,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawCones(g);	
	glutSwapBuffers();
}
