#include "cuberobot.h"
#include <cmath>

PLYModel * CubeRobot::cube = NULL ;
PLYModel * CubeRobot::hind = NULL ;

using namespace std;

CubeRobot :: CubeRobot ( void ) {
    // Optional , uncomment if you want a local origin marker !
    // SetMesh ( cube );
    fly = new SceneNode ( hind , Vector4(1 ,1 ,0 ,1)); // Yellow+Checker !
    fly -> translateV = Vector3 (0, 4,-8);
    fly -> relativePos = Vector3 (0, 4, -8);
    fly -> scaleV = Vector3 (1.5 ,1.5 ,1.5);
    fly -> rotateX = -90;
    fly_target = Vector3 (5 ,2 ,4);
    fly -> model->textureON = true;
    fly -> textureID = 0;
    AddChild ( fly );

    body = new SceneNode ( cube , Vector4(1 ,0 ,0 ,1)); // Red !
    body -> translateV = Vector3 (0 ,0 ,0);
    body -> relativePos = Vector3 (0, 0, 0);
    body -> scaleV = Vector3 (1,1.5 ,0.5);
    AddChild ( body );

    head = new SceneNode ( cube , Vector4 (0 ,1 ,0 ,1)); // Green !
    head -> translateV = Vector3 (0.25 ,1.5 ,0);
    head -> relativePos = Vector3 (0.25, 1.5, 0);
    head -> scaleV = Vector3 (.5 ,.5 ,.5);
    body -> AddChild ( head );

    leftArm = new SceneNode ( cube , Vector4 (0 ,0 ,1 ,1)); // Blue !
    leftArm -> translateV = Vector3 ( -.25 , 1.25, 0);
    leftArm -> relativePos = Vector3 ( -.25 , 1.25, 0);
    leftArm -> scaleV = Vector3 (0.25, -1.25 ,0.25);
    body -> AddChild ( leftArm );

    rightArm = new SceneNode ( cube , Vector4 (0 ,0 ,1 ,1)); // Blue !
    rightArm -> translateV = Vector3 ( 1 , 1.25, 0);
    rightArm -> relativePos = Vector3 ( 1 , 1.25, 0);
    rightArm -> scaleV = Vector3 (0.25, -1.25 ,0.25);
    body -> AddChild ( rightArm );

    rightLeg = new SceneNode ( cube , Vector4 (0 ,0 ,1 ,1)); // Blue !
    rightLeg -> translateV = Vector3 ( .6 , 0, .125);
    rightLeg -> relativePos = Vector3 ( .6 , 0, .125);
    rightLeg -> scaleV = Vector3 (0.25, -1.5 ,0.25);
    body -> AddChild ( rightLeg );

    leftLeg = new SceneNode ( cube , Vector4 (0 ,0 ,1 ,1)); // Blue !
    leftLeg -> translateV = Vector3 ( .2 , 0, .125);
    leftLeg -> relativePos = Vector3 ( .2 , 0, .125);
    leftLeg -> scaleV = Vector3 (0.25, -1.5 ,0.25);
    body ->  AddChild ( leftLeg );

    body -> relativePos += Vector3 (0,1.5,0);
    SceneNode :: Update ( 0 );
}

/* Logic to animate the scene
   Scene: A fly is moving randomly in space. A robot moves in a straight line. when fly comes
          near it, fly comes sit on his head.
          Both moves together. A keyboard key makes it release it. Both moves independently.
*/
void CubeRobot :: Update ( float msec ) {

    if(! CubeRobot::is_attached) {
        if(!CubeRobot::is_root_child) {
            fly -> relativePos =  head->translateV;
            AddChild(fly);
            head -> RemoveChild(fly);
            is_root_child = true;
        }
        // Move fly randomly
        // Check if reached target
        float distance = (fly->translateV - fly_target).Length();
        if(distance < 0.1) {
            float x = (-0.5 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*12;
            float z = (-0.5 + static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*12;
            float y = 4 + (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*3;
            fly_target = Vector3(x,y,z);
        }
        Vector3 direction = (fly_target - fly->translateV);
        direction.Normalise();
        fly -> relativePos += direction*DroneSpeed;
        float angle = atan2(direction.x,direction.y) * 180 / 3.141592654;
        if(rotateZ!=angle)
            fly -> rotateZ += DroneRotSpeed;
    }
    else {
        float distance = (fly->translateV - fly_target).Length();
        if(distance < .5) {
            if(CubeRobot::is_root_child){
                fly -> relativePos =  Vector3(0,0,0);
                head -> AddChild(fly);
                RemoveChild(fly);
                is_root_child = false;
            }
        }
        else{
            fly_target = head->translateV + Vector3(0,.5,0);
            Vector3 direction = (fly_target - fly->translateV);
            direction.Normalise();
            fly -> relativePos += direction*DroneSpeed;
            float angle = atan2(direction.x,direction.y) * 180 / 3.141592654;
            if(rotateZ!=angle)
                fly -> rotateZ += DroneRotSpeed;
        }
    }

    // Move robot in a straight line
    body -> relativePos.z =  sin(msec/100.0)*RobotSpeed;

    float rotationAngle = sin(msec/10.0)*45.0;
    leftArm -> rotateX = rotationAngle;
    rightLeg -> rotateX = rotationAngle;
    rightArm -> rotateX = -rotationAngle;
    leftLeg -> rotateX = -rotationAngle;

    eye = fly->translateV;
    center = body->translateV;

    SceneNode :: Update ( msec );
}

bool CubeRobot::is_attached = false;
bool CubeRobot::is_root_child = true;
Vector3 CubeRobot::eye = Vector3(0,0,0);
Vector3 CubeRobot::center =  Vector3(0,0,0);
float CubeRobot::RobotSpeed = 6;
float CubeRobot::DroneSpeed = 0.1;
float CubeRobot::DroneRotSpeed = 2;
