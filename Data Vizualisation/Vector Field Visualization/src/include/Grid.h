#pragma once


class ScalarAttributes;
class VectorAttributes;


class Grid
{
public: 

virtual			         ~Grid();
virtual int				  numPoints()			=0; 
virtual int				  numCells()			=0; 
virtual void			  getPoint(int,float*)	=0; 
virtual int				  getCell(int,int*)		=0; 
virtual int				  findCell(float*)		=0;
virtual ScalarAttributes& pointScalars()		=0;
virtual VectorAttributes& pointVectors()		=0;

virtual bool			  getC1Scalar(float* position, float& value) =0;
						  //Compute scalar value at given position, using bilinear interpolation
						  //Returns true if position inside grid, else false (interpolation failed)

virtual bool			  getC1Vector(float* position, float* value) =0;
						  //Compute vector value at given position, using bilinear interpolation
						  //Returns true if position inside grid, else false (interpolation failed)

protected:
						  Grid() { }
};



