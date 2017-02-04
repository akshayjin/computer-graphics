#include <QtWidgets>
#include <QtOpenGL>
#include <QOpenGLFunctions_1_5>
#include <GL/glu.h>
#include <iostream>
#include <QTimer>
#include"cuberobot.h"

#include "myglwidget.h"

MyGLWidget::MyGLWidget(QWidget *parent) :
    QGLWidget( parent)
{
    xTran = 0;
    yTran = 0;
    zTran = 0;
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    timer->start(30);
}

MyGLWidget::~MyGLWidget()
{
}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(750, 750);
}

QSize MyGLWidget::sizeHint() const
{
    return QSize(750, 750);
}

/*
 *  GUI Event Handlers
 * */

void MyGLWidget::setXTranslation(int translation)
{
    xTran = translation;
    updateGL();
}

void MyGLWidget::setYTranslation(int translation)
{
    yTran = translation;
    updateGL();
}

void MyGLWidget::setZTranslation(int translation)
{
    zTran = translation;
    updateGL();
}

void MyGLWidget::setRobotSpeed(int speed)
{
    CubeRobot::RobotSpeed = speed;
    updateGL();
}

void MyGLWidget::setDroneSpeed(int speed)
{
    CubeRobot::DroneSpeed = speed/10.0;
    updateGL();
}

void MyGLWidget::setDroneRotSpeed(int speed)
{
    CubeRobot::DroneRotSpeed = speed;
    updateGL();
}

void MyGLWidget::setZoom(int factor)
{
    scalef = factor/4.0;
    updateGL();
}

void MyGLWidget::toggleLight0(bool on)
{
    light0 = on;
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT2);
    glLoadIdentity();
    updateGL();
}

void MyGLWidget::toggleLight1(bool on)
{
    light1 = on;
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT2);
    glLoadIdentity();
    updateGL();
}

void MyGLWidget::toggleLight2(bool on)
{
    light2 = on;
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT2);
    glLoadIdentity();
    updateGL();
}

void MyGLWidget::toggleLight3(bool on)
{
    light3 = on;
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT2);
    glLoadIdentity();
    updateGL();
}

void MyGLWidget::toggleBB(bool on)
{
//    model.model3D_1.bb = on;
//    model.model3D_2.bb = on;
    model.ToggleAttachment();
    updateGL();
}

void MyGLWidget::toggleTex1(bool on)
{
    model.model3D_1.textureON = on;
    updateGL();
}

void MyGLWidget::toggleTex2(bool on)
{
    model.model3D_2.textureON = on;
    updateGL();
}

void MyGLWidget::changeTex1()
{
   model.model3D_1_texture = (model.model3D_1_texture+1)%model.texture_ids.size();
   updateGL();
}

void MyGLWidget::changeTex2()
{
   model.model3D_2_texture = (model.model3D_2_texture+1)%model.texture_ids.size();
   updateGL();
}

/*
 * Set OpenGL Context
 * */

void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::black);
    model.init();
}

/*
 * Clear Canvas and enable depth buffering
 * */
void MyGLWidget::paintGL()
{
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth( 1.0 );
    draw();
    glLoadIdentity();
    swapBuffers();
}

/*
 * Handle window resize. Called once at the beginning
 * */

void MyGLWidget::resizeGL(int width, int height)
{
    screenW=width;
    screenH=height;
    glMatrixMode (GL_PROJECTION);						// Set the projection matrix
    glLoadIdentity ();
    gluPerspective(fov,float(width)/height,z_near,z_far);
//    gluLookAt(eye_x,eye_y,eye_z,c_x,c_y,c_z,up_x,up_y,up_z);
    glViewport(0,0,width,height);								// Set the viewport to the entire size of the rendering window
    glMatrixMode(GL_MODELVIEW);							// Set the modelview matrix (including the camera position and view direction)
}

/*
 *  Mouse left button is attached to trackball
 * */
void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
    if (event->buttons() & Qt::LeftButton){
        rotateInit(event->x(),event->y());
    }
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        rotate(event->x(),event->y());
        updateGL();
    } else if (event->buttons() & Qt::RightButton) {
    }

    lastPos = event->pos();
}

// Main OpenGL Display function

void MyGLWidget::draw()
{
    if(cameraPosition==0)
    {
        static float rotation_transform[4][4] ;
        trackball.rotationMatrix( rotation_transform ) ;

        float eye[]={eye_x,eye_y,eye_z,1};
        float up[]={up_x,up_y,up_z,0};
        float new_eye[4],new_up[4];
        new_eye[3]=1;
        for (int i=0;i<4;i++){
            new_eye[i]=0;
            new_up[i]=0;
            for (int j=0;j<4;j++){
                new_eye[i]+=( rotation_transform[i][j]*eye[j]);
                new_up[i]+=( rotation_transform[i][j]*up[j]);
            }
        }
        gluLookAt(new_eye[0],new_eye[1],new_eye[2],c_x,c_y,c_z,new_up[0],new_up[1],new_up[2]);
        glColor3f(1.0, 1.0, 1.0);
        glScalef(scalef,scalef,scalef);
        glTranslatef(xTran,yTran,zTran);
        model.render();
        setLight(new_eye);
    }
    else
    {
        Vector3 eye = model.getEye();
        Vector3 center = model.getCenter();

        glLoadIdentity();
        gluLookAt(eye.x,eye.y,eye.z,center.x,center.y,center.z,0,1,0);
        glPushMatrix();
            glColor3f(1.0, 1.0, 1.0);
            model.render();
            float eye2[]={eye.x,eye.y,eye.z,1};
            setLight(eye2);
        glPopMatrix();
    }
    ticks = ((int)(ticks+1));
    model.UpdateScene(ticks);
}

void MyGLWidget::rotateInit(int x,int y)
{
    x_ang = x ;
    y_ang = y ;
}

void MyGLWidget::rotate(int x,int y)
{
    trackball.rotate((2.0 * x_ang - screenW) / screenW,
        -(screenH - 2.0 * y_ang) / screenH,
        (2.0 * x - screenW) / screenW,
        -(screenH - 2.0 * y) / screenH);
    x_ang = x ; y_ang = y ;
}

void MyGLWidget::scale(float times)
{
    scalef *= times;
}

void MyGLWidget::setLight(float eye[4])
{
    if(light0)
    {
        // Add Light Source which moves with camera (attached to camera lens)
        glPushMatrix();
//            glLoadIdentity();
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize( 10.0 );
            glShadeModel(GL_SMOOTH);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);
            glLightfv(GL_LIGHT0,GL_POSITION,eye);
            glEnable(GL_LIGHT0);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glBegin( GL_POINTS );
                glColor3f( 1,1,0 );
                glVertex3f( eye[0],eye[1],eye[2] );
            glEnd();
        glPopMatrix();
    }
    if(light1)
    {
        // Add Spotlight Source
        glPushMatrix();
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize( 10.0 );
            glShadeModel(GL_SMOOTH);
            // Light values and coordinates
            GLfloat  lightPos[] = { 2.5f, 5.0f, 1.0f, 1.0f };
            GLfloat  specular[] = { 0.0f, 0.0f, 0.0f, 1.0f};
            GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
            GLfloat  ambientLight[] = { 0.5f, 0.5f, 0.0f, 1.0f};
            Vector3 dir = Vector3(-2,-1.5,-0.5);
            dir.Normalise();
            GLfloat  spotDir[] = { 0,1,0 };

            // Hidden surface removal
            glEnable(GL_DEPTH_TEST);

            // Enable lighting
            glEnable(GL_LIGHTING);


            // Set up and enable light 1
            // Supply a slight ambient light so the objects can be seen
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
//            glLightfv(GL_LIGHT1,GL_AMBIENT,ambientLight);
            glLightfv(GL_LIGHT1,GL_DIFFUSE,ambientLight);
            glLightfv(GL_LIGHT1,GL_SPECULAR,specular);
            glLightfv(GL_LIGHT1,GL_POSITION,lightPos);

            // Specific spot effects
            glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
            glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,20.0f);
            glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,2.0f);

            // Enable this light in particular
            glEnable(GL_LIGHT1);

            // Enable color tracking
            glEnable(GL_COLOR_MATERIAL);

            // Set Material properties to follow glColor values
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

            // All materials hereafter have full specular reflectivity
            // with a high shine
            glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
            glMateriali(GL_FRONT, GL_SHININESS,128);

            // Adding a point at light location
            glBegin( GL_POINTS );
                glColor3f( 1,1,0 );
                glVertex3f( lightPos[0],lightPos[1],lightPos[2] );
            glEnd();
            glLoadIdentity();

        glPopMatrix();
    }
    if(light2)
    {
        glPushMatrix();
            glEnable(GL_PROGRAM_POINT_SIZE);
            glPointSize( 10.0 );
            glShadeModel(GL_SMOOTH);
            Vector3 eye = model.getEye();
            GLfloat  lightPos[] = { -3.0f, 2.0f, 0.5f, 1.0f };
            GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
            GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
            GLfloat  ambientLight[] = { .5f, 0.5f, 0.5f, 1.0f};

            Vector3 dir = Vector3(eye.x-lightPos[0],eye.y-lightPos[1],eye.z-lightPos[2]);
            dir.Normalise();
            GLfloat  spotDir[] = { dir.x,dir.y,dir.z };
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);

            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
            glLightfv(GL_LIGHT2,GL_DIFFUSE,ambientLight);
            glLightfv(GL_LIGHT2,GL_SPECULAR,specular);
            glLightfv(GL_LIGHT2,GL_POSITION,lightPos);
            glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir);
            glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,10.0f);
            glLightf(GL_LIGHT2,GL_SPOT_EXPONENT,1.0f);
            glEnable(GL_LIGHT2);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
            glMateriali(GL_FRONT, GL_SHININESS,128);

            glBegin( GL_POINTS );
                glColor3f( 1,1,0 );
                glVertex3f( lightPos[0],lightPos[1],lightPos[2] );
            glEnd();
        glPopMatrix();
    }
    if(light3)
    {
        glPushMatrix();
            glShadeModel(GL_SMOOTH);
            GLfloat  ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f};
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_LIGHTING);

            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
            glEnable(GL_LIGHT3);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
        glPopMatrix();
    }
}

void MyGLWidget::toggleCamera()
{
    cameraPosition = (cameraPosition+1)%2;
    updateGL();
}

void MyGLWidget::setCamera()
{
}
