#include "include/VectorAttributes.h"
#include <algorithm>

using namespace std;



float VectorAttributes::getMin()
{
	float minimum = values[0].norm();
	for(int i=1;i<values.size();++i)
	   minimum = min(minimum,values[i].norm());
	   
	return minimum;
}

float VectorAttributes::getMax()
{
	float maximum = values[0].norm();
	for(int i=1;i<values.size();++i)
	   maximum = max(maximum,values[i].norm());
	   
	return maximum;
}
	   

