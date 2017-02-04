#ifndef PROGRESSIVE_MESH_H
#define PROGRESSIVE_MESH_H

#include "vector.h"
#include "list.h"

class tridata {
  public:
	int	v[3];  // indices to vertex list
	int	t[3];  // indices to texture list
};

extern int NUM_CHARTS;

void ProgressiveMesh(List<Vector> &vert,  List<tridata> &tri, 
                     List<int> &map,  List<int> &permutation, List<int> &chart_map, int &boundary_len );

#endif
