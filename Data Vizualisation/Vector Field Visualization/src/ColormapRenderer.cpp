#include "include/ColormapRenderer.h"
#include "include/ScalarAttributes.h"
#include "include/Grid.h"
#include <GL/glut.h>									//GLUT library
#include <algorithm>
#include <math.h>


using namespace std;



void rainbow(float value,float& R,float& G,float& B)	//Maps a scalar in [0,1] to a color using the rainbow colormap
{
   const float dx=0.8f;

   value = (6-2*dx)*value+dx;
   R = max(0.0f,(3-(float)fabs(value-4)-(float)fabs(value-5))/2);
   G = max(0.0f,(4-(float)fabs(value-2)-(float)fabs(value-4))/2);
   B = max(0.0f,(3-(float)fabs(value-1)-(float)fabs(value-2))/2);
}

void grayscale(float value,float& R,float& G,float& B)	//Maps a scalar in [0,1] to a color using a grayscale colormap
{
   R = G = B = value;
}

void saturation(float value,float& R,float& G,float& B)	//Maps a scalar in [0,1] to a color using a saturation colormap
{
   float color_r = 0;									//The base color whose saturation we change (green).	
   float color_g = 1;									//Try different colors!	
   float color_b = 0;
   
   if (value<0.5)										//value in [0,0.5]: modulate the luminance from black to the base-color.
   {   
	   R = 2*value*color_r;
	   G = 2*value*color_g;
	   B = 2*value*color_b;
   }
   else													//value in [0.5,1]: modulate the saturation from base-color to white.
   {
	   value = 2*(value-0.5);	
	   R = (1-value)*color_r + 1*value;
	   G = (1-value)*color_g + 1*value;
	   B = (1-value)*color_b + 1*value;
   }
}


void ColormapRenderer::mapToColor(float value,float& R,float& G,float& B)
{														//Map v to a color using currently active colormap
	//~ switch (colormap_type)
	//~ {
	//~ case COLORMAP_GRAYSCALE: 
		//~ grayscale(value,R,G,B);
		//~ break;
	//~ case COLORMAP_RAINBOW: 
		//~ rainbow(value,R,G,B);
		//~ break;
	//~ case COLORMAP_SATURATION: 
		//~ saturation(value,R,G,B);
		//~ break;
	//~ }
	rainbow(value,R,G,B);
}		

void ColormapRenderer::drawGrid(Grid& g)				//Draw 'g' as simple grid. Compare this code with SimpleRenderer.
{
	glColor3f(0,0,0);									//1.   Use black to render cells
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);			//     Render cells as outlines

	for(int i=0;i<g.numCells();++i)						//2. Draw all cells in the grid as black outlines
	{
		int   cell[10];									//Should have enough storage space for our largest cell
		float x[2];
		
		int nvertices = g.getCell(i,cell);				//2.1. Get the 'nvertices' vertex-IDs of i-th cell
		if (nvertices!=3 && nvertices!=4)				//     We only handle here drawing of triangle and quad cells.
		   continue;									//     It is quite simple to extend this to other cell types.

		if (nvertices==3)								//     Triangle cells:
			glBegin(GL_TRIANGLES);						
		else //nvertices==4	
			glBegin(GL_QUADS);							//     Quad cells:
		
		for(int j=0;j<nvertices;++j)					//2.2. Render current cell
		{
			g.getPoint(cell[j],x);						//     Get vertex coordinates of j-th vertex of i-th cell		
			glVertex3f(x[0],x[1],0);					//     Pass this coordinate to OpenGL
		}	
		glEnd();                                        //2.3. Finish rendering current cell
	}

	glPointSize(3);										//3.   Draw all vertices in the grid as large red points
	glColor3f(1,0,0);
	glBegin(GL_POINTS);									//3.1. Draw vertices as an OpenGL point-set	
	for(int i=0;i<g.numPoints();++i)						
	{
		float v[2];
		g.getPoint(i,v);								//3.2. Get coordinates of i-th vertex				
		glVertex3f(v[0],v[1],0);						//     Pass this coordinate to OpenGL
	}
	glEnd();											//3.3. Finish rendering the entire point-set
}



void ColormapRenderer::drawPoints(Grid& g)				//Draw 'g' by showing scalar values at their definition locations (vertices)
{
	ScalarAttributes& scalars = g.pointScalars();		//     Get scalar range, to normalize scalar-values next in [0,1]
	float scalar_min = scalars.getMin();
	float scalar_max = scalars.getMax();
	
	glPointSize(3);										//1.   Draw all vertices in the grid as large points
	glBegin(GL_POINTS);									//2.   Draw vertices as an OpenGL point-set	
	for(int i=0;i<g.numPoints();++i)						
	{
		float v[2];
		g.getPoint(i,v);								//2.1. Get coordinates of i-th vertex	
		
		float val = (g.pointScalars().getC0Scalar(i)-scalar_min)/(scalar_max-scalar_min);
		float r,g,b;									//2.2.  Get scalar value at i-th vertex and normalize it to [0,1]
		mapToColor(val,r,g,b);							//2.3.  Map normalized scalar value to a color 
		glColor3f(r,g,b);								//2.4.  Pass color to OpenGL
		glVertex3f(v[0],v[1],0);						//2.5.  Pass this coordinate to OpenGL
	}
	glEnd();											//3.    Finish rendering the entire point-set
}


void ColormapRenderer::drawC0Cells(Grid& g)				//Draw 'g' by showing C0-interpolated scalar values over cells.
{														//Cell scalars are computed from vertex scalar by averaging.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//1.    Render cells filled
	glShadeModel(GL_FLAT);								//2.    Do not interpolate vertex colors (use flat-shaded polygons)

	ScalarAttributes& scalars = g.pointScalars();		//3.    Get scalar range, to normalize scalar-values next in [0,1]
	float scalar_min = scalars.getMin();
	float scalar_max = scalars.getMax();

	for(int i=0;i<g.numCells();++i)						//4. Draw all cells in the grid as filled flat-shaded polygons
	{
		int   cell[10];									//Should have enough storage space for our largest cell
		
		int nvertices = g.getCell(i,cell);				//4.1. Get the 'nvertices' vertex-IDs of i-th cell
		if (nvertices!=3 && nvertices!=4)				//     We only handle here drawing of triangle and quad cells.
		   continue;									//     It is quite simple to extend this to other cell types.

		if (nvertices==3)								//     Triangle cells:
			glBegin(GL_TRIANGLES);						
		else //nvertices==4	
			glBegin(GL_QUADS);							//     Quad cells:
		

		float average_val = 0;							//4.2. Compute average scalar over the current cell	
		for(int j=0;j<nvertices;++j)					//
			average_val += g.pointScalars().getC0Scalar(cell[j]);
		average_val /= nvertices;	
														//4.3. Normalize average scalar in [0,1] range
		average_val = (average_val-scalar_min)/(scalar_max-scalar_min);		

		float R,G,B;									//4.4. Map average scalar to color
		mapToColor(average_val,R,G,B);					//4.5. Pass color to OpenGL. As we use flat-shading, the entire cell
		glColor3f(R,G,B);								//     will have this color	

		for(int j=0;j<nvertices;++j)					//4.6. Render current cell
		{
			float x[2];
			g.getPoint(cell[j],x);						//     Get vertex coordinates of j-th vertex of i-th cell				
			glVertex3f(x[0],x[1],0);					//     Pass this coordinate to OpenGL
		}	
		glEnd();                                        //4.7. Finish rendering current cell
	}
}


void ColormapRenderer::drawC1Cells(Grid& g)				//Draw 'g' by showing C1-interpolated scalars over cells
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//1.   Render cells afilled
	glShadeModel(GL_SMOOTH);							//2.   Use bilinear interpolation when drawing cells as polygons

	ScalarAttributes& scalars = g.pointScalars();		//3.    Get scalar range, to normalize scalar-values next in [0,1]
	float scalar_min = scalars.getMin();
	float scalar_max = scalars.getMax();

	for(int i=0;i<g.numCells();++i)						//4. Draw all cells in the grid filled bilinearly-interpolated polygons
	{
		int   cell[10];									//Should have enough storage space for our largest cell
		
		int nvertices = g.getCell(i,cell);				//4.1. Get the 'nvertices' vertex-IDs of i-th cell
		if (nvertices!=3 && nvertices!=4)				//     We only handle here drawing of triangle and quad cells.
		   continue;									//     It is quite simple to extend this to other cell types.

		if (nvertices==3)								//     Triangle cells:
			glBegin(GL_TRIANGLES);						
		else //nvertices==4	
			glBegin(GL_QUADS);							//     Quad cells:
		
		for(int j=0;j<nvertices;++j)					//4.2. Render current cell
		{
			float x[2];
			g.getPoint(cell[j],x);						//4.3. Get vertex coordinates of j-th vertex of i-th cell	
						
			float value = (g.pointScalars().getC0Scalar(cell[j])-scalar_min)/(scalar_max-scalar_min);
														//4.4. Get vertex scalar value, normalize it in [0,1]    	
			float R,G,B;								
			mapToColor(value,R,G,B);						//4.5. Map normalized scalar to color
			glColor3f(R,G,B);							//4.6. Pass vertex color to OpenGL
			glVertex3f(x[0],x[1],0);					//4.7. Pass vertex coordinate to OpenGL
		}	
		glEnd();                                        //4.8. Finish rendering current cell
	}
}



void ColormapRenderer::draw(Grid& g)					//Draw grid 'g' depending on drawing style attribute-value
{
	glClearColor(1,1,1,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	switch (draw_style)
	{
	case DRAW_GRID: 
		drawGrid(g);
		break;
	case DRAW_POINTS:
		drawPoints(g);
		break;
	case DRAW_C0_CELLS:
		drawC0Cells(g);
		break;
	case DRAW_C1_CELLS:
		drawC1Cells(g);
		break;
	}			
	
	glutSwapBuffers();
}




