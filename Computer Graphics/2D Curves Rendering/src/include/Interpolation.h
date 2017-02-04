#pragma once

#include <vector>
#include "Point2D.h"
#include <math.h>

class Interpolation
{
	public:
		//Lagrange Interpolating Polynomial
		float LIP(std::vector<Point2D> points,int n,int x)
		{
			float Px = 0;
			for(int j=0;j<n;j++)
			{
				float Pjx = points[j].y;
				for(int k=0;k<n;k++)
				{
					if(k!=j)
					{
						Pjx *= (x-points[k].x)/(points[j].x-points[k].x);
					}
				}
				Px += Pjx;
			}
			return Px;
		}
		
		Point2D Bezier_curve(std::vector<Point2D> points,float t)
		{
			float sumx = 0;
			float sumy = 0;
			int n = points.size();
			for(int i=0;i<n;i++)
			{
				sumx += nCr(n-1,i) * pow(1-t,n-i-1) * pow(t,i) * points[i].x;
			}
			for(int i=0;i<n;i++)
			{
				sumy += nCr(n-1,i) * pow(1-t,n-i-1) * pow(t,i) * points[i].y;
			}
			return Point2D(sumx,sumy);
		}
		
		int nCr(int n,int r)
		{
			int numerator = 1;
			int denominator = 1;
			for(int i=r+1;i<=n;i++)
				numerator *= i;
			for(int i=2;i<=n-r;i++)
				denominator *= i;
			return numerator/denominator;
		}
};
