#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <iostream>

using namespace std;

void Model::render()
{
	drawAxis();
	for(Point2D p: points)
	{
		p.render();
	}
	if(points.size()>=2)
	{
		line_lagrange.render();
		line_bezier.render();
	}
}

void Model::drawAxis()
{
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(10,10,0);
		glVertex3f(50,10,0);
		glVertex3f(50,10,0);
		glVertex3f(40,5,0);
		glVertex3f(40,5,0);
		glVertex3f(40,15,0);
		glVertex3f(40,15,0);
		glVertex3f(50,10,0);
		
		glColor3f(0,1,0);
		glVertex3f(10,10,0);
		glVertex3f(10,50,0);
		glVertex3f(10,50,0);
		glVertex3f(5,40,0);
		glVertex3f(5,40,0);
		glVertex3f(15,40,0);
		glVertex3f(15,40,0);
		glVertex3f(10,50,0);
	glEnd();
}

void Model::rotate(float deg)
{
	float sumX=0,sumY=0;
	for(Point2D p:points)
	{
		sumX+=p.x;
		sumY+=p.y;
	}
	float xpos = sumX/points.size();
	float ypos = sumY/points.size();
	
	glTranslatef(xpos, ypos, 0);
	glRotatef(deg, 0.0f, 0.0f, 1.0f);
	glTranslatef(-xpos,-ypos,0);
}

void Model::scale(float times)
{
	glScalef(times,times,1);
}

void Model::translate(float x, float y)
{
	glTranslatef(x/20,-y/20,0);
}

void Model::addPoint(Point2D point)
{
	points.push_back(point);
	line_bezier.addPoint(point);
	line_bezier.calculate();
	line_lagrange.addPoint(point);
	line_lagrange.calculate();
}

void Model::deletePoint(Point2D point)
{
	int ndx = -1;
	for(int i=0;i<points.size();i++)
	{
		if(point.dist(points[i]) <= 10)
		{
			ndx = i;
			break;
		}
	}
	if(ndx!=-1)
	{
		points.erase(points.begin()+ndx);
		line_bezier.deletePoint(ndx);
		line_bezier.calculate();
		line_lagrange.deletePoint(ndx);
		line_lagrange.calculate();
	}
}
