#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <iostream>

using namespace std;

extern Model model;
extern View view;

bool Controller::mouseButtonPressed[5] = {false,false,false,false,false};

void Controller::run()
{
	glutDisplayFunc(view.draw);
	glutReshapeFunc(view.viewing);
	glutKeyboardFunc(KeyDown);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseDrag);
	glutMainLoop();
}

void Controller::mouseFunc(int button, int state, int x, int y)
{
	switch (button) 
	{
	case 0:
		if(state == GLUT_DOWN)
		{
			mouseButtonPressed[0] = true;
			view.rotateInit(x,y);
			glutPostRedisplay();
		}
		else
			mouseButtonPressed[0] = false;
			view.trackball.rotate( 0,0,0,0 ) ;
			glutPostRedisplay();
		break;
	case 2:
		if(state == GLUT_DOWN)
		{
			previousX = x;
			previousY = y;
			mouseButtonPressed[2] = true;
		}
		else
			mouseButtonPressed[2] = false;
		break;
	case 1:
		if(state == GLUT_DOWN)
		{
			mouseButtonPressed[1] = true;
		}
		else
			mouseButtonPressed[1] = false;
	case 3:	
		glutPostRedisplay();
		break;
	case 4:
		glutPostRedisplay();
		break;
	}
}

void Controller::mouseDrag(int x, int y){
	if(mouseButtonPressed[0]){
		view.rotate(x,y);
		glutPostRedisplay();
	}
	if(mouseButtonPressed[2]){
		model.translate(x-previousX,y-previousY);
		glutPostRedisplay();
	}
}

void Controller::KeyDown(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 27:		//ESC
		exit(0);
		break;
	case '+':
		model.scale(1.1);
		glutPostRedisplay();
		break;
	case '-':
		model.scale(0.9);
		glutPostRedisplay();
		break;
	case 'l':		
		model.rotate(10);
		glutPostRedisplay();
		break;
	case 'r':		
		model.rotate(-10);
		glutPostRedisplay();
		break;
	}
}

float Controller::previousX = 0.0;
float Controller::previousY = 0.0;
