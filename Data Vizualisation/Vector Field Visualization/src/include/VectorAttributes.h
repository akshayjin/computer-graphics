#pragma once

#include <vector>
#include "Point3d.h"

using namespace std;


class VectorAttributes
{
public:
				VectorAttributes(int size):values(size) { }
void			setC0Vector(int i,float* n)
				{  values[i] = Point3d(n); }
float*			getC0Vector(int i)
				{  return values[i].data;  }	
float			getMin();
float			getMax();
				
protected:

vector<Point3d>	values;
};
