#ifndef SCENENODE_H
#define SCENENODE_H

#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "plymodel.h"
#include <vector>

class SceneNode {
public :
    SceneNode ( PLYModel * m = NULL , Vector4 colour = Vector4 (1 ,1 ,1 ,1));
    ~ SceneNode ( void );

    Vector4 GetColour () const { return colour ;}
    void SetColour ( Vector4 c ) { colour = c ;}

    PLYModel * GetPLYModel () const { return model ;}
    void SetPLYModel ( PLYModel * m ) { model = m ;}

    void AddChild ( SceneNode * s );
    void RemoveChild ( SceneNode * s );

    virtual void Update ( float msec );
    virtual void Draw ();
    std :: vector < SceneNode * >:: const_iterator GetChildIteratorStart () {
    return children.begin ();}

    std :: vector < SceneNode * >:: const_iterator GetChildIteratorEnd () {
    return children.end ();}

    Vector3 translateV ;
    Vector3 relativePos;
    Vector3 scaleV ;
    float rotateX;
    float rotateY;
    float rotateZ;
    int textureID;

    SceneNode * parent ;
    PLYModel * model ;
protected :

    Vector4 colour ;
    std :: vector < SceneNode * > children ;
 };

#endif // SCENENODE_H
