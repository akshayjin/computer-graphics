#pragma once

class View
{
private:
	static float Xmin;
	static float Xmax;
	static float Ymin;
	static float Ymax;
	static float screenW;
	static float screenH;
public:
	static void draw();
	static void viewing(int W, int H);
	static float getScreenH() { return screenH; }
};
