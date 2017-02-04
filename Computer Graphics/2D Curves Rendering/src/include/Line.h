#pragma once

#include <vector>
#include "Point2D.h"
#include "Interpolation.h"
#include <GL/glut.h>
#include <algorithm>

#define MAX_POINTS_LINE 100

class Line
{
	private:
		std::vector <Point2D> points;
		std::vector <Point2D> pointsToRender;
		int interpolation_method = 0;
	
	public:
		//Lagrange Interpolating Polynomial
		Line(int im)
		{
			interpolation_method = im;
		}
		
		void addPoint(Point2D point)
		{
			points.push_back(point);
		}
		
		void deletePoint(int ndx)
		{
			points.erase(points.begin()+ndx);
		}
		
		void calculate()
		{
			pointsToRender.clear();	
			std::vector <Point2D> points = this->points;
			std::sort(points.begin(),points.end());
			if(interpolation_method==0)
			{
				Interpolation interpolation_formulas;
				for(int i=0;i<points.size();i++)
				{
					float x = points[i].x;
					float y = points[i].y;
					float interval = (points[i+1].x - points[i].x)/(MAX_POINTS_LINE/points.size());
					while(x<points[i+1].x)
					{
						Point2D p(x,interpolation_formulas.LIP(points,points.size(),x));
						pointsToRender.push_back(p);
						x += interval;
					}
				}
			}
			else
			{
				Interpolation interpolation_formulas;
				for(float t=0;t<=1;t+=0.01)
				{
					pointsToRender.push_back(interpolation_formulas.Bezier_curve(points,t));
				}
			}
		}
		
		void render()
		{
			glBegin(GL_LINES);
			if(interpolation_method==0)
				glColor3f(0,1,0);
			else
				glColor3f(0,0,1);
			for(int i=0;i<pointsToRender.size()-1;i++)
			{
				glVertex3f(pointsToRender[i].x,pointsToRender[i].y,0);
				glVertex3f(pointsToRender[i+1].x,pointsToRender[i+1].y,0);
			}
			glEnd();
		}
};

