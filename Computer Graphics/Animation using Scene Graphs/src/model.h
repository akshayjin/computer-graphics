#ifndef MODEL_H
#define MODEL_H

#include "plymodel.h"
#include "scenenode.h"
#include <vector>
#include <string>

class Model
{
public:
    void init();
    Model();
    ~Model();
    PLYModel model3D_1;
    PLYModel model3D_2;
    void render();
    void drawAxis();
    void drawFloor();
    void rotate(float deg);
    void scale(float times);
    void translate(float x, float y);
    GLuint loadBMP_custom(const char * imagepath);
    std::vector<std::string> texture_files;
    static std::vector<GLuint> texture_ids;
    int model3D_1_texture = 2;
    int model3D_2_texture = 1;

    SceneNode * root ;
    void DrawNode ( SceneNode * n );
    virtual void UpdateScene ( float msec );
    void ToggleAttachment();

    Vector3 getEye();
    Vector3 getCenter();
};


#endif // MODEL_H
