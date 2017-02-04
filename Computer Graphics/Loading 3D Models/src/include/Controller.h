#pragma once

#include <GL/glut.h>	
#include "Model.h"
#include "View.h"

class Controller
{
private:
	static bool mouseButtonPressed[5];
	static float previousX;
	static float previousY;
public:
	void run();
	static void mouseFunc(int button, int state, int x, int y);
	static void mouseDrag(int x, int y);
	static void KeyDown(unsigned char key, int x, int y);
};
