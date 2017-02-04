#include "include/UniformGrid.h"
#include <math.h>
#include <iostream>

using namespace std;


UniformGrid::~UniformGrid()
{  }


int	UniformGrid::getCell(int i,int* v)
{  
	int cell_row = i / (N1-1);
	int cell_col = i % (N1-1);
	
    v[0] = i + cell_row;
	v[1] = v[0]+1;
	v[2] = v[1]+N1;
	v[3] = v[0]+N1;
	
	return 4;
}

int UniformGrid::findCell(float* p)
{
	int C[2];
	
	//compute cell coordinates C[0],C[1]
	C[0] = floor((p[0]-m1)/d1);
	C[1] = floor((p[1]-m2)/d2);

	//test if p is inside the dataset
	if (C[0]<0 || C[0]>=N1-1 || C[1]<0 || C[1]>=N2-1)
	  return -1;

	//go from cell coordinates to cell index
	return C[0] + C[1]*(N1-1);
}


bool UniformGrid::getC1Scalar(float* position, float& value)
{
	int c = findCell(position);				//Find cell that the point 'position' falls into
	if (c==-1) return false;				//Cell outside dataset? Nothing to interpolate there
	
	int cell[4];
	getCell(c,cell);						//Get indices of four cell vertices

	float p0[2],p1[2],p2[2],p3[2];			//Get coordinates of four cell vertices
	getPoint(cell[0],p0);
	getPoint(cell[1],p1);
	getPoint(cell[2],p2);
	getPoint(cell[3],p3);

	float s0,s1,s2,s3;						//Get scalar values at four cell vertices
	s0 = scalars.getC0Scalar(cell[0]);
	s1 = scalars.getC0Scalar(cell[1]);
	s2 = scalars.getC0Scalar(cell[2]);
	s3 = scalars.getC0Scalar(cell[3]);

	float tx = fabs(position[0]-p0[0])/d1;	//Parametric coordinates of point within cell
	float ty = fabs(position[1]-p0[1])/d2;
	
	value=0;								//Compute scalar value at given point, using
											//bilinear interpolation of the scalar values at
	value += (1-tx)*(1-ty)*s0;				//the four cell vertices	
	value += tx*(1-ty)*s1;
	value += tx*ty*s2;
	value += (1-tx)*ty*s3;
	
	return true;							//Interpolation succeeded
}


bool UniformGrid::getC1Vector(float* position, float* value)
{
	int c = findCell(position);				//Find cell that the point 'position' falls into
	if (c==-1) return false;				//Cell outside dataset? Nothing to interpolate there
	
	int cell[4];
	getCell(c,cell);						//Get indices of four cell vertices

	float p0[2],p1[2],p2[2],p3[2];			//Get coordinates of four cell vertices
	getPoint(cell[0],p0);
	getPoint(cell[1],p1);
	getPoint(cell[2],p2);
	getPoint(cell[3],p3);

	float *v0,*v1,*v2,*v3;					//Get vector values at four cell vertices
	v0 = vectors.getC0Vector(cell[0]);
	v1 = vectors.getC0Vector(cell[1]);
	v2 = vectors.getC0Vector(cell[2]);
	v3 = vectors.getC0Vector(cell[3]);

	float tx = fabs(position[0]-p0[0])/d1;	//Parametric coordinates of point within cell
	float ty = fabs(position[1]-p0[1])/d2;
	
	value[0]=value[1]=0;					//Compute vector value at given point, using
											//bilinear interpolation of the vector values at
	value[0] += (1-tx)*(1-ty)*v0[0];		//the four cell vertices	
	value[1] += (1-tx)*(1-ty)*v0[1];

	value[0] += tx*(1-ty)*v1[0];
	value[1] += tx*(1-ty)*v1[1];

	value[0] += tx*ty*v2[0];
	value[1] += tx*ty*v2[1];

	value[0] += (1-tx)*ty*v3[0];
	value[1] += (1-tx)*ty*v3[1];
	
	return true;							//Interpolation succeeded
}

	
