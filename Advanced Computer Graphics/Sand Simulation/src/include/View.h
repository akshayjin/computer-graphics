#pragma once
#include "FastTrackball.h"

class View
{
private:
	static float Xmin;
	static float Xmax;
	static float Ymin;
	static float Ymax;
	static float screenW;
	static float screenH;
	static float x_ang;
	static float y_ang;
	static float eye_x, eye_y, eye_z;				//Modelview (camera extrinsic) parameters
	static float c_x, c_y, c_z;
	static float up_x, up_y, up_z;
	static float fov;										//Perspective projection parameters
	static float aspect; 
	static float z_near; 
	static float z_far;
public:
	static void draw();
	static Trackball trackball ;
	static void viewing(int W, int H);
	static void setCamera();
	static float getScreenH() { return screenH; }
	static void rotateInit(int x,int y);
	static void rotate(int x,int y);
};
