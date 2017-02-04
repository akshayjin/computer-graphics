#ifndef CUBEROBOT_H
#define CUBEROBOT_H

#pragma once
#include "scenenode.h"
#include "plymodel.h"
#include <cstdlib>
#include <ctime>

class CubeRobot : public SceneNode {
public :
    CubeRobot ( void );
    ~CubeRobot ( void ){}
    virtual void Update ( float msec );

    static void CreateCube () {
        PLYModel * m = new PLYModel ();
        m->Load ("cube2.ply");
        PLYModel * n = new PLYModel ();
        n->Load ("hind.ply");
        cube = m ;
        hind = n ;
        srand (static_cast <unsigned> (time(0)));
    }
    static void DeleteCube (){ delete cube ;}
    static void ToggleAttachment() {is_attached = !is_attached;}
    Vector3 fly_target;
    static bool is_attached;
    static bool is_root_child;

    static Vector3 eye;
    static Vector3 center;
    static float RobotSpeed;
    static float DroneSpeed;
    static float DroneRotSpeed;

protected :
    static PLYModel * cube ;
    static PLYModel * hind;
    SceneNode * body;
    SceneNode * head ;
    SceneNode * leftArm ;
    SceneNode * rightArm ;
    SceneNode * leftLeg ;
    SceneNode * rightLeg ;
    SceneNode * fly;
};


#endif // CUBEROBOT_H
