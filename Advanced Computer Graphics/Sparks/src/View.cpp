#include <GL/glut.h>
#include <GL/gl.h>
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <iostream>
#include <math.h>

using namespace std;

extern Model model;

void View::draw()
{
	glClearColor(0,0,0,1);								//1. Clear the frame and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearDepth( 1.0 );
	static float rotation_transform[4][4] ;
	trackball.rotationMatrix( rotation_transform ) ;
	
	float eye[]={eye_x,eye_y,eye_z,1};
	float up[]={up_x,up_y,up_z,0};
	float new_eye[4],new_up[4];
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
	model.render();
	setLight(new_eye);
	glLoadIdentity();
	glutSwapBuffers() ;
}

void View::viewing(int W, int H)
{
	screenW=W;
	screenH=H;
	glMatrixMode (GL_PROJECTION);						// Set the projection matrix
	glLoadIdentity ();
	gluPerspective(fov,float(W)/H,z_near,z_far);
	
	gluLookAt(eye_x,eye_y,eye_z,c_x,c_y,c_z,up_x,up_y,up_z);
	glViewport(0,0,W,H);								// Set the viewport to the entire size of the rendering window
	glMatrixMode(GL_MODELVIEW);							// Set the modelview matrix (including the camera position and view direction)
	if(model.texture_ids.size()==0)
		model.init();
}

void View::rotateInit(int x,int y)
{
	x_ang = x ;
	y_ang = y ;
}

void View::rotate(int x,int y)
{
	trackball.rotate((2.0 * x_ang - screenW) / screenW,
		-(screenH - 2.0 * y_ang) / screenH,
		(2.0 * x - screenW) / screenW,
		-(screenH - 2.0 * y) / screenH);
	x_ang = x ; y_ang = y ;
}

void View::setLight(float eye[4])
{
	if(light0)
	{
		// Add Light Source
		glPushMatrix();
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
		// Add Light Source
		glPushMatrix();
			glEnable(GL_PROGRAM_POINT_SIZE);
			glPointSize( 10.0 );
			glShadeModel(GL_SMOOTH);				
			GLfloat  lightPos[] = { 2.0f, 1.5f, 0.5f, 1.0f };
			GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
			GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
			GLfloat  ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f};
			GLfloat  spotDir[] = { .5f, -1.5f, 1.0f };	
			glEnable(GL_DEPTH_TEST);  
			glEnable(GL_LIGHTING);	
			
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
			glLightfv(GL_LIGHT1,GL_DIFFUSE,ambientLight);
			glLightfv(GL_LIGHT1,GL_SPECULAR,specular);
			glLightfv(GL_LIGHT1,GL_POSITION,lightPos);
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotDir);
			glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,60.0f);
			glLightf(GL_LIGHT1,GL_SPOT_EXPONENT,5.0f);
			glEnable(GL_LIGHT1);
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
			glMaterialfv(GL_FRONT, GL_SPECULAR,specref);
			glMateriali(GL_FRONT, GL_SHININESS,128);
			
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
			GLfloat  lightPos[] = { -3.0f, 1.5f, 0.5f, 1.0f };
			GLfloat  specular[] = { 1.0f, 1.0f, 1.0f, 1.0f};
			GLfloat  specref[] =  { 1.0f, 1.0f, 1.0f, 1.0f };
			GLfloat  ambientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f};
			GLfloat  spotDir[] = { 1.5f, -1.5f, 1.0f };	
			glEnable(GL_DEPTH_TEST);  
			glEnable(GL_LIGHTING);	
			
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
			glLightfv(GL_LIGHT2,GL_DIFFUSE,ambientLight);
			glLightfv(GL_LIGHT2,GL_SPECULAR,specular);
			glLightfv(GL_LIGHT2,GL_POSITION,lightPos);
			glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spotDir);
			glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,60.0f);
			glLightf(GL_LIGHT2,GL_SPOT_EXPONENT,10.0f);
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
			GLfloat  ambientLight[] = { 1.0f, 1.0f, 1.0f, 1.0f};
			glEnable(GL_DEPTH_TEST);  
			glEnable(GL_LIGHTING);	
			
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
			glEnable(GL_LIGHT3);
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glPopMatrix();
	}
}

void View::scale(float times)
{
	scalef *= times;
}

float View::screenW = 600.0;
float View::screenH = 600.0;
float View::eye_x = 0.05;
float View::eye_y = .1;
float View::eye_z = 3;
float View::c_x = 0;
float View::c_y = 0;
float View::c_z = 0;
float View::up_x = 0;
float View::up_y = 1;
float View::up_z = 0;
float View::fov	 = 90;										//Perspective projection parameters
float View::aspect = 1; 
float View::z_near = 0.1; 
float View::z_far  = 1000;
float View::x_ang;
float View::y_ang;
bool View::light0 = true;	// Camera
bool View::light1 = false;	// Model 2
bool View::light2 = false;	// Model 1
bool View::light3 = true;	// Ambient Light
float View::scalef = 1;
Trackball View::trackball;
