#include "scenenode.h"

#include "scenenode.h"
#include <iostream>
#include <GL/gl.h>
#include "model.h"

using namespace std;

SceneNode :: SceneNode (PLYModel * model , Vector4 colour ) {
    this->model = model ;
    this->colour = colour ;
    parent = NULL;
    translateV = Vector3 (0 ,0 ,0);
    relativePos = Vector3 (0 ,0 ,0);
    scaleV = Vector3 (1 ,1 ,1);
    rotateX = 0;
    rotateY = 0;
    rotateZ = 0;
    textureID = -1;
}

SceneNode ::~ SceneNode ( void ) {
    for ( unsigned int i = 0; i < children.size (); ++ i ) {
        delete children[i];
    }
}

void SceneNode::AddChild ( SceneNode * s ) {
    children.push_back ( s );
    s->parent = this ;
}

void SceneNode::RemoveChild ( SceneNode * s ) {
    children.push_back ( s );
    children.erase(std::remove(children.begin(), children.end(), s), children.end());
}

void SceneNode::Draw () {
    glColor3f(colour.x,colour.y,colour.z);
    if( model ) {
        if(textureID==-1)
            model->Draw(0);
        else
            model->Draw(Model::texture_ids[textureID]);
    }
    glColor3f(1.0,1.0,1.0);
}

void SceneNode::Update(float msec){
    if(parent){//This node has a parent...
        this->translateV = parent->translateV+this->relativePos;
//        scaleV = parent->scaleV*this->scaleV;
//        this->rotationV = parent->rotationV;
    }
    else{//Root node , world transform is local transform!
        this->translateV = this->relativePos;
    }
    for(vector <SceneNode*> :: iterator i = children.begin(); i != children.end(); ++i){
        (*i)->Update(msec);
    }
}
