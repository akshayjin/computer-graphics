#include "include/StreamlineRenderer.h"
#include "include/VectorAttributes.h"
#include "include/Grid.h"
#include <GL/glut.h>									//GLUT library





void StreamlineRenderer::traceEuler(Grid& g,const Point3d& seed,vector<StreamPoint>& streamline)				
{														//Trace a single streamline from 'seed' downstream

	VectorAttributes& vectors = g.pointVectors();		//Get vector range, to normalize vector lengths next in [0,1]
	float vector_min = vectors.getMin();
	float vector_max = vectors.getMax();
	
	Point3d current = seed;
	
	float streamline_length = 0;						//Integrate the vector field for a given maximal length:
	for(int i=0;streamline_length<max_length;++i,streamline_length+=step_size)
	{	
		float v[3]; v[2]=0;								//1. Get vector field at current position, using bilinear interpolation 
		bool inside = g.getC1Vector(current.data,v);		
		if (!inside) break;								//2. If current position is outside the grid, stop the tracing

		Point3d vec(v);
		float length  = vec.norm();
		
		StreamPoint p;									//3. Create a new streamline-point. Besides position, save the	
		p.point     = current;							//normalized vector length, and also the distance along the streamline
		p.magnitude = (length-vector_min)/(vector_max-vector_min);
		p.position  = streamline_length;
		
		streamline.push_back(p);						//4. Append point to streamline

		//~ if (length<1.0e-5) break;						//5. If we reach an area of near-zero vector magnitude, stop tracing
				
		vec /= length;									//6. Generate a new point by simple Euler integration. 
														//   Use here the normalized vector, for an even point distibution
		current += vec*step_size;						//   along the streamline
	}
	
	for(int i=0;i<streamline.size();++i)				//After the streamline is ready, normalize the distance along the streamline
	{													//in the range [0,1]. We will use this next for shading purposes.
		StreamPoint& p = streamline[i];
		p.position /= streamline_length;
	}
}

void StreamlineRenderer::traceRK4(Grid& g,const Point3d& seed,vector<StreamPoint>& streamline)				
{														//Trace a single streamline from 'seed' downstream

	VectorAttributes& vectors = g.pointVectors();		//Get vector range, to normalize vector lengths next in [0,1]
	float vector_min = vectors.getMin();
	float vector_max = vectors.getMax();
	
	Point3d current = seed;
	
	float streamline_length = 0;						//Integrate the vector field for a given maximal length:
	for(int i=0;streamline_length<max_length;++i,streamline_length+=step_size)
	{	
		float v[3]; v[2]=0;								//1. Get vector field at current position, using bilinear interpolation 
		bool inside = g.getC1Vector(current.data,v);		
		if (!inside) break;								//2. If current position is outside the grid, stop the tracing

		Point3d vec(v);
		float length  = vec.norm();
		
		StreamPoint p;									//3. Create a new streamline-point. Besides position, save the	
		p.point     = current;							//normalized vector length, and also the distance along the streamline
		p.magnitude = (length-vector_min)/(vector_max-vector_min);
		p.position  = streamline_length;
		
		streamline.push_back(p);						//4. Append point to streamline
		
		Point3d k1(vec*step_size);
		
		inside = g.getC1Vector(current+k1*0.5,v);		
		if (!inside) break;	
		Point3d k2(v);
		k2 = k2*step_size;
		
		inside = g.getC1Vector(current+k2*0.5,v);		
		if (!inside) break;	
		Point3d k3(v);
		k3 = k3*step_size;
		
		inside = g.getC1Vector(current+k3,v);		
		if (!inside) break;	
		Point3d k4(v);
		k4 = k4*step_size;
			
		if (length<1.0e-5) break;						//5. If we reach an area of near-zero vector magnitude, stop tracing
				
		vec /= length;									//6. Generate a new point by simple Euler integration. 
		k1/=k1.norm();k2/=k2.norm();k3/=k3.norm();k4/=k4.norm();	
														//   Use here the normalized vector, for an even point distibution
		current = current + (k1+k2*2.0)/6.0 + (k3*2.0+k4)/6.0;						//   along the streamline
	}
	
	for(int i=0;i<streamline.size();++i)				//After the streamline is ready, normalize the distance along the streamline
	{													//in the range [0,1]. We will use this next for shading purposes.
		StreamPoint& p = streamline[i];
		p.position /= streamline_length;
	}
}


void StreamlineRenderer::draw(Grid& g)					//Draw streamlines for grid 'g' depending on drawing style attribute-value
{
	glClearColor(1,1,1,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for(int i=0;i<seeds.size();++i)						//2. Trace and draw a streamline for each seed point:
	{
		vector<StreamPoint> streamline;
		const Point3d& seed = seeds[i];

		traceRK4(g,seed,streamline);						//2.1. Trace the streamline. Collect result in 'streamline'
		
		switch(draw_style)								//2.2. Draw traced streamline depending on drawing style
		{
		case DRAW_POINTS:
			drawPoints(streamline);
			break;
		case DRAW_LINES:
			drawLines(streamline);
			break;
		case DRAW_MAGNITUDE_LINES:
			drawMagnitudeLines(streamline);	
			break;
		case DRAW_SHADED_LINES:
			drawShadedLines(streamline);	
			break;
		}
	}
	
	glutSwapBuffers();
}



void StreamlineRenderer::drawPoints(const vector<StreamPoint>& streamline)
{														//Draw streamline as a point set, colored in black
	glPointSize(thickness);
	glEnable(GL_BLEND);									//Enable OpenGL point-smoothing for high-quality point drawing
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	
	glColor3f(0,0,0);
	glBegin(GL_POINTS);
	for(int i=0;i<streamline.size();++i)
	{
		const StreamPoint& p = streamline[i];
		glVertex2f(p.point.x,p.point.y);
	}
	glEnd();	
}


void StreamlineRenderer::drawLines(const vector<StreamPoint>& streamline)
{														//Draw streamline as a polyline, colored in black
	glLineWidth(thickness);
	glEnable(GL_BLEND);									//Enable OpenGL line-smoothing for high-quality line drawing	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	glColor3f(0,0,0);	
	glBegin(GL_LINE_STRIP);
	for(int i=0;i<streamline.size();++i)
	{
		const StreamPoint& p = streamline[i];
		glVertex2f(p.point.x,p.point.y);
	}
	glEnd();	
}


void StreamlineRenderer::drawShadedLines(const vector<StreamPoint>& streamline)
{														//Draw streamline as a polyline, with transparency linearly
	glLineWidth(thickness);								//decreasing along the streamline. This creates a nice arrow-like effect,
	glEnable(GL_BLEND);									//when using thick lines
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	glBegin(GL_LINE_STRIP);
	for(int i=0;i<streamline.size();++i)
	{
		const StreamPoint& p = streamline[i];		
		glColor4f(0,0,0,1-p.position);					//Set transparency as function of normalized distance along streamline
		glVertex2f(p.point.x,p.point.y);
	}
	glEnd();	
}


void StreamlineRenderer::drawMagnitudeLines(const vector<StreamPoint>& streamline)
{														//Draw streamline as a polyline, colored by vector magnitude
	glLineWidth(thickness);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	
	setColormapType(COLORMAP_RAINBOW);					//Try here other colormaps from ColormapRenderer!
	
	glBegin(GL_LINE_STRIP);
	for(int i=0;i<streamline.size();++i)
	{
		const StreamPoint& p = streamline[i];		
		float r,g,b;									
		mapToColor(p.magnitude,r,g,b);				
		glColor3f(r,g,b);		
		glVertex2f(p.point.x,p.point.y);
	}
	glEnd();	
}





