#pragma once

#include "ColormapRenderer.h"
#include "Point3d.h"
#include <vector>


using namespace std;



class StreamlineRenderer : public ColormapRenderer
{
public:

enum DRAW_STYLE											//Different streamline drawing styles
{
	DRAW_LINES = 0,										// -black lines
	DRAW_POINTS,										// -black points evenly spread along streamline
	DRAW_MAGNITUDE_LINES,								// -lines colored by vector magnitude
	DRAW_SHADED_LINES									// -black lines with decreasing transparency
};	

				StreamlineRenderer(): draw_style(DRAW_LINES),max_length(1),step_size(0.1),thickness(1) {}

void			setSeeds(const vector<Point3d>& s)
				{ seeds = s; }

void			setDrawingStyle(DRAW_STYLE s)
				{ draw_style = s; }

void			setMaxLength(float l)
				{ max_length = l; }			

float			getMaxLength()
				{ return max_length; }			

void			setStepSize(float s)
				{ step_size = s; }			

float			getStepSize()
				{ return step_size; }
				
void			setThickness(float t)
				{ thickness = t; }			

float			getThickness()
				{ return thickness; }			

void			draw(Grid&);


protected:

struct StreamPoint										//A point along a computed streamline. 
{
				StreamPoint() {}
Point3d			point;									//Point position
float			magnitude;								//Vector magnitude at current point
float			position;								//Position of point along streamline (in [0,1] arc-length coordinates)
};

DRAW_STYLE		draw_style;
float			max_length;
float			step_size;
float			thickness;

vector<Point3d> seeds;

void			traceEuler(Grid& g,const Point3d& seed,vector<StreamPoint>& streamline);				
void			traceRK4(Grid& g,const Point3d& seed,vector<StreamPoint>& streamline);

private:

void			drawPoints(const vector<StreamPoint>& streamline);
void			drawLines(const vector<StreamPoint>& streamline);
void			drawMagnitudeLines(const vector<StreamPoint>& streamline);
void			drawShadedLines(const vector<StreamPoint>& streamline);
};




						

