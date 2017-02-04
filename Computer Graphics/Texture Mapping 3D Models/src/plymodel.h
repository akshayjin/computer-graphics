#ifndef PLYMODEL_H
#define PLYMODEL_H

#include <QtOpenGL>

class PLYModel
{
public:
    int Load(char *filename);
    void Draw(GLuint Texture);
    void setCenter(float cx,float cy,float cz);
    float* calculateNormal( float *coord1, float *coord2, float *coord3 );
    PLYModel();
    float x_min,x_max,y_min,y_max,z_min,z_max;
    float* Faces_Triangles;
    float* Vertex_Buffer;
    float* Normals;
    float* Faces_TextureUV;
    int TotalConnectedTriangles;
    int TotalConnectedPoints;
    int TotalFaces;
    float centerX,centerY,centerZ;
    void drawBB();
    bool bb = true;
    int texture_fun = 0; //0-> cylindrical 1-> spherical
    bool textureON = true;
};


#endif // PLYMODEL_H
