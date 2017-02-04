#pragma once
 
class PLYModel 
{
public:
	int Load(char *filename);
	void Draw();
	float* calculateNormal( float *coord1, float *coord2, float *coord3 );
	PLYModel();
	float x_min,x_max,y_min,y_max,z_min,z_max;
    float* Faces_Triangles;
	float* Vertex_Buffer;
	float* Normals;
	int TotalConnectedTriangles;	
	int TotalConnectedPoints;
	int TotalFaces;
};
