#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <iostream>
#include "include/pngwriter.h"

using namespace std;

static int frame_no = 0;
static int width = 600;
static int height = 600;
static int npixels = width*height*3;
static GLfloat* OpenGLimage = new GLfloat[npixels];

extern Model model;

void View::draw()
{
	glClearColor(0,0,0,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth( 1.0 );
	glPushMatrix();
		static float rotation_transform[4][4] ;
		trackball.rotationMatrix( rotation_transform ) ;
		glMultMatrixf( &rotation_transform[0][0] ) ;
		glColor3f(1.0, 1.0, 1.0);
		model.render();
	glPopMatrix();
	
	glReadPixels(0.0, 0.0, width, height,GL_RGB, GL_FLOAT, OpenGLimage);
	char fname[32];
	sprintf(fname,"frames/%d.png",frame_no);
	pngwriter PNG(width, height, 1.0, fname);
	size_t x = 1;   // start the top and leftmost point of the window
	size_t y = 1;
	double R, G, B;
	for(size_t i=0; i<npixels; i++)
	{
		  switch(i%3) //the OpenGLimage array look like [R1, G1, B1, R2, G2, B2,...]
		 {
			   case 2:
					 B = (double) OpenGLimage[i]; break;
			   case 1:
					 G = (double) OpenGLimage[i]; break;
			   case 0:
					 R = (double) OpenGLimage[i];
					 PNG.plot(x, y, R, G, B);
					 if( x == width )
					 {
						   x=1;
						   y++;
					  }
					  else
					  { x++; }
					  break;
		 }
	}
	PNG.close();
	glutSwapBuffers();
	frame_no++;
}

void View::viewing(int W, int H)
{
	glEnable(GL_DEPTH_TEST);
	screenW=W;
	screenH=H;
	glMatrixMode (GL_PROJECTION);						// Set the projection matrix
	glLoadIdentity ();
	gluPerspective(fov,float(W)/H,z_near,z_far);
	
	gluLookAt(eye_x,eye_y,eye_z,c_x,c_y,c_z,up_x,up_y,up_z);
	glViewport(0,0,W,H);								// Set the viewport to the entire size of the rendering window
	glMatrixMode(GL_MODELVIEW);							// Set the modelview matrix (including the camera position and view direction)
}

void View::rotateInit(int x,int y)
{
	x_ang = x ;
	y_ang = y ;
}

void View::rotate(int x,int y)
{
	trackball.rotate((2.0 * x_ang - screenW) / screenW,
		(screenH - 2.0 * y_ang) / screenH,
		(2.0 * x - screenW) / screenW,
		(screenH - 2.0 * y) / screenH);
	x_ang = x ; y_ang = y ;
}

float View::screenW = 600.0;
float View::screenH = 600.0;
float View::eye_x = 3;
float View::eye_y = 5;
float View::eye_z = -0.5;
float View::c_x = 0;
float View::c_y = 0;
float View::c_z = 0;
float View::up_x = 0;
float View::up_y = 1;
float View::up_z = 0;
float View::fov	 = 120;										//Perspective projection parameters
float View::aspect = 1; 
float View::z_near = 0.1; 
float View::z_far  = 1000;
float View::x_ang;
float View::y_ang;
Trackball View::trackball;
