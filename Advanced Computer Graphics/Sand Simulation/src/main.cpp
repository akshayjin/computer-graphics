#include <iostream>
#include <GL/glut.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <string>

using namespace std;

Model model;
View view;

void printHelp()
{
	cout << "User << interaction : " << "HELP" << endl;
	cout << "Zoom in: " << "+" << endl;
	cout << "Zoom out: " << "-" << endl;
	cout << "Rotate :" << "left click and drag" << endl;
	cout << "Translate : " << "right click + drag" << endl;

}

int main(int argc,char* argv[])
{
	printHelp();
	Controller controller;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);			//2. Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
	glutInitWindowSize(600,600);							//3. Tell GLUT how large are the windows we want to create next
	glutCreateWindow("Assignment 3: Particle Based Simulation of Granular Particles");
	controller.run();
	return 0;	
}
