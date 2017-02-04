#ifndef VECTOR_H
#define VECTOR_H

#include "math.h"
#include "stdio.h"

#define EPSZERO 1E-12

class Vector
{
public:

    inline Vector(const double cx=0, const double cy=0, const double cz=0)
    {
	coord[0] = cx;
	coord[1] = cy;
        coord[2] = cz;
    }

    inline Vector(const Vector &v)
    {
	coord[0] = v.coord[0];
	coord[1] = v.coord[1];
	coord[2] = v.coord[2];
    }

    inline Vector &operator=(const Vector &v)
    {
	coord[0] = v.coord[0];
	coord[1] = v.coord[1];
	coord[2] = v.coord[2];

	return *this;
    }

    
    inline double operator,(const Vector &v1) const
    {
	return
	    (v1.coord[0] * coord[0] +
	     v1.coord[1] * coord[1] +
	     v1.coord[2] * coord[2]);
    }

    inline Vector operator*(const Vector &v1) const
    {
	return
	    Vector (coord[1] * v1.coord[2] -
		    v1.coord[1] * coord[2],
		    coord[2] * v1.coord[0] -
		    v1.coord[2] * coord[0],
		    coord[0] * v1.coord[1] -
		    v1.coord[0] * coord[1]);
    }

    inline Vector operator*(const double c) const
    {
	return
	    Vector (coord[0]*c,
		    coord[1]*c,
		    coord[2]*c);
    }

    inline Vector &operator+=(const Vector &v1)
    {
	coord[0] += v1.coord[0];
	coord[1] += v1.coord[1];
	coord[2] += v1.coord[2];

	return *this;
    }

    inline Vector &operator-=(const Vector &v1)
    {
	coord[0] -= v1.coord[0];
	coord[1] -= v1.coord[1];
	coord[2] -= v1.coord[2];

	return *this;
    }
    
    inline Vector &operator*=(const double c)
    {
	coord[0] *= c;
	coord[1] *= c;
	coord[2] *= c;

	return *this;
    }

    inline Vector &operator/=(double c)
    {
	c = 1./c;
	
	coord[0] *= c;
	coord[1] *= c;
	coord[2] *= c;

	return *this;
    }
    
    inline Vector operator+(const Vector &v1) const
    {
	return Vector(coord[0] + v1.coord[0],
		      coord[1] + v1.coord[1],
		      coord[2] + v1.coord[2]);
    }

    inline Vector operator-(const Vector &v1) const
    {

	return Vector(coord[0] - v1.coord[0],
		      coord[1] - v1.coord[1],
		      coord[2] - v1.coord[2]);
    }

    inline Vector operator-() const
    {

	return Vector(-coord[0],
		      -coord[1],
		      -coord[2]);
    }

    inline Vector operator/(double c) const
    {
	c = 1./c;

	return Vector(c*coord[0],
		      c*coord[1],
		      c*coord[2]);
    }

    inline double sizesqd() const
    {
	return
	    coord[0]*coord[0] +
	    coord[1]*coord[1] +
	    coord[2]*coord[2];
    }

    inline double size() const
    {
	return
	    sqrt(sizesqd());
    }
    
    inline Vector unit() const
    {
	return (*this)/size();
    }
    
    void Print() {
	printf("(%lf,%lf,%lf)\n",coord[0],coord[1],coord[2]);
    }

  inline int iszero() const
  {
    if (sizesqd() < EPSZERO) 
      return 1;
    return 0;
  }
    
    double coord[3];
    
};

inline double distance(const Vector &v1, const Vector &v2)
{
    return (v1-v2).size();
}

inline double dsqrd(const Vector &v1, const Vector &v2)
{
    return (v1-v2).sizesqd();
}

#endif
