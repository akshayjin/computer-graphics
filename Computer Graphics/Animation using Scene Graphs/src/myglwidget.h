#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>
#include "model.h"
#include "trackball.h"

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    ~MyGLWidget();
    Model model;
protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void rotate(int x,int y);
    void rotateInit(int x,int y);
    void setLight(float eye[4]);
    void scale(float times);

signals:

public slots:
    // slots for xyz-rotation slider
    void setXTranslation(int translation);
    void setYTranslation(int translation);
    void setZTranslation(int translation);
    void setRobotSpeed(int speed);
    void setDroneSpeed(int speed);
    void setDroneRotSpeed(int speed);

    // Scales the model
    void setZoom(int factor);
    void setCamera();
    // ON/OFF Switch for lights, bounding boxes and textures
    void toggleLight0(bool on);
    void toggleLight1(bool on);
    void toggleLight2(bool on);
    void toggleLight3(bool on);
    void toggleBB(bool on);
    void toggleTex1(bool on);
    void toggleTex2(bool on);
    void changeTex1();
    void changeTex2();
    void toggleCamera();

signals:
    // <UNUSED> signaling rotation from mouse movement
    //(only needed when multiple ways to change world)
    void xTranslationChanged(int translation);
    void yTranslationChanged(int translation);
    void zTranslationChanged(int translation);

private:
    void draw();

    //Stores Translation
    int xTran;
    int yTran;
    int zTran;
    // Stores scaling factor
    int zoomf;
    // Mouse coordinates for trackball
    float x_ang;
    float y_ang;
    int screenW = 700;
    int screenH = 700;
    // Trackball for rotation
    Trackball trackball ;

    //Initialising camera parameters and lights
    float eye_x = 0.05, eye_y = 1, eye_z = 3;				//Modelview (camera extrinsic) parameters
    float c_x = 0, c_y = 0, c_z = 0;
    float up_x = 0, up_y= 1, up_z = 0;
    float fov = 90;										//Perspective projection parameters
    float z_near = 0.1;
    float z_far = 50;
    float scalef = 1;
    bool light0 = true;
    bool light1 = false;
    bool light2 = false;
    bool light3 = true;

    int cameraPosition = 0;

    float ticks  = 0;

    QPoint lastPos;

};

#endif // MYGLWIDGET_H
