#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"

using namespace std;

extern Model model;

void View::draw()
{
	glClearColor(0,0,0,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glPointSize( 10.0 );
	model.render();
	glutSwapBuffers();
}

void View::viewing(int W, int H)
{
	glMatrixMode(GL_MODELVIEW);							//1. Set the modelview matrix (including the camera position and view direction)
	glLoadIdentity ();										

	glMatrixMode (GL_PROJECTION);						//2. Set the projection matrix
	glLoadIdentity ();
	gluOrtho2D(0,W,0,H);
	glDisable(GL_DEPTH_TEST);
	
	screenW = W;
	screenH = H;
	
	glViewport(0,0,W,H);		
}

float View::Xmin = 0.0;
float View::Xmax = 600.0;
float View::Ymin = 0.0;
float View::Ymax = 600.0;
float View::screenW = 600.0;
float View::screenH = 600.0;
