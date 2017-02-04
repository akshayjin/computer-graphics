#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <iostream>

using namespace std;

void Model::render()
{
	// Plot PLY file
	model3D.Draw();
	
	// Draw wireframes
	drawBoundingBox();
	glLoadIdentity ();
	drawAxis();
	
	// Add Light Source
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize( 10.0 );
	glShadeModel(GL_SMOOTH);						
	float light[4] = {1,1,1,0};	
	glBegin( GL_POINTS );
		glColor3f( 1,1,0 );
		glVertex3f( light[0],light[1],light[2] );
	glEnd();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0,GL_POSITION,light);
	glEnable(GL_COLOR_MATERIAL);
}

void Model::drawBoundingBox()
{
	float xl = .5;
	float scale = xl*2 / (model3D.x_max - model3D.x_min);
	float yl = (model3D.y_max - model3D.y_min) * scale / 2;
	float zl = (model3D.z_max - model3D.z_min) * scale / 2;
	
	glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
	glBegin(GL_QUADS);
		
		glColor3f(0,1,1);
		glVertex3f(-1-xl,-1-yl,-1-zl);
		glVertex3f(-1-xl,1+yl,-1-zl);
		glVertex3f(-1-xl,1+yl,1+zl);
		glVertex3f(-1-xl,-1-yl,1+zl);
		
		glVertex3f(1+xl,-1-yl,-1-zl);
		glVertex3f(1+xl,1+yl,-1-zl);
		glVertex3f(1+xl,1+yl,1+zl);
		glVertex3f(1+xl,-1-yl,1+zl);
		
		glVertex3f(-1-xl,-1-yl,-1-zl);
		glVertex3f(1+xl,-1-yl,-1-zl);
		glVertex3f(1+xl,-1-yl,1+zl);
		glVertex3f(-1-xl,-1-yl,1+zl);
		
		glVertex3f(-1-xl,1+yl,-1-zl);
		glVertex3f(1+xl,1+yl,-1-zl);
		glVertex3f(1+xl,1+yl,1+zl);
		glVertex3f(-1-xl,1+yl,1+zl);
		
	glEnd();
}
void Model::drawAxis()
{
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(-1000,0,0);
		glVertex3f(1000,0,0);
		glColor3f(0,1,0);
		glVertex3f(0,-1000,0);
		glVertex3f(0,1000,0);
		glColor3f(0,0,1);
		glVertex3f(0,0,-1000);
		glVertex3f(0,0,1000);
	glEnd();
}

void Model::rotate(float deg)
{
}

void Model::scale(float times)
{
	glScalef(times,times,times);
}

void Model::translate(float x, float y)
{
	glTranslatef(x/5000,-y/5000,0);
}
