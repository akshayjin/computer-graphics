#include <iostream>
#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"

using namespace std;

Model model;
View view;

void printHelp()
{
	cout << "User interaction : " << "HELP" << endl;
	cout << "Zoom in: " << "+" << endl;
	cout << "Zoom out: " << "-" << endl;
	cout << "Rotate clockwise :" << "l" << endl;
	cout << "Rotate anticlockwise : " << "r" << endl;
	cout << "Translate : " << "right click + drag" << endl;
	cout << "Add point : " << "left click" << endl;
	cout << "Delete Point : " << "middle mouseclick on point" << endl;

}

int main(int argc,char* argv[])
{
	printHelp();
	Controller controller;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);			//2. Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
	glutInitWindowSize(600,600);							//3. Tell GLUT how large are the windows we want to create next
	glutCreateWindow("Assignment 1: Interpolation and Tranformation");
	controller.run();
	return 0;	
}
