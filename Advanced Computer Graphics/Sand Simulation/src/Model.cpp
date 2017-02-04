#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>  

using namespace std;

Model::Model():particleSystem(10000){srand (time(NULL));}

void Model::render()
{
	// Add Light Source
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize( 10.0 );
	glShadeModel(GL_FLAT);						
	float light[4] = {0,11,1,0};	
	glBegin( GL_POINTS );
		glColor3f( 1,1,0 );
		glVertex3f( light[0],light[1],light[2] );
	glEnd();
	
	particleSystem.render();
	particleSystem.update();
	// Draw wireframes
	drawBoundingBox();
	//~ glLoadIdentity ();
	drawAxis();
}

void Model::drawBoundingBox()
{
	glColor3f(1,1,1);
	glBegin(GL_POLYGON);
		glVertex3f(-6,0,-6);
		glVertex3f(-6,0,6);
		glVertex3f(6,0,6);
		glVertex3f(6,0,-6);
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
