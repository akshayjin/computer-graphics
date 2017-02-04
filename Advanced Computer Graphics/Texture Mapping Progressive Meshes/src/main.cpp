#include <GL/glut.h>
#include <GL/gl.h>	
#include <GL/glu.h>	
#include <iostream>


#include "vector.h"

using namespace std;

// Functions and Variables from Model module
extern void       InitModel();
extern void       RenderModel();
extern void       KeyDown(unsigned char key, int x, int y);
extern Vector     model_position;    
extern Quaternion model_orientation;

int 	Width  = 512;
int 	Height = 512;
int     MouseX = 0;
int     MouseY = 0;
Vector  MouseVector;      
Vector  OldMouseVector;
int     MouseState=0;   
float 	ViewAngle=45.0f;
bool mouseButtonPressed[5];

void Reshape(int width, int height){
	Width=width;
	Height=height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(ViewAngle, (float)width/height, 0.1, 50.0);
    glMatrixMode(GL_MODELVIEW);	
    glLoadIdentity();
}

void Display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
	glLoadIdentity();
	RenderModel();
	glLoadIdentity();
    glPopMatrix();
    glFlush();
    glutSwapBuffers() ;
}

void ComputeMouseVector(){
	OldMouseVector=MouseVector;
	float spread = (float)tan(ViewAngle/2*3.14/180);  
	float y = spread * ((Height-MouseY)-Height/2.0f) /(Height/2.0f);
    float x = spread * (MouseX-Width/2.0f)  /(Height/2.0f);
    Vector v(x ,y,-1); 
    v=normalize(v);
	MouseVector = v;
}

Quaternion VirtualTrackBall(Vector cop,Vector cor,Vector dir1,Vector dir2) {
	float m;
	Vector nrml = cor - cop;
	float fudgefactor = 1.0f/(magnitude(nrml) * 0.25f); 
	nrml = normalize(nrml);
	float dist = -(nrml^cor);
	Vector u= planelineintersection(nrml,dist,cop,cop+dir1);
	u=u-cor;
	u=u*fudgefactor;
	m= magnitude(u);
	if(m>1) {u=u*1.0f/m;}
	else {
		u=u - (nrml * (float)sqrt(1-m*m));
	}
	Vector v= planelineintersection(nrml,dist,cop,cop+dir2);
	v=v-cor;
	v=v*fudgefactor;
	m= magnitude(v);
	if(m>1) {v=v*1.0f/m;}
	else {
		v=v - (nrml * (float)sqrt(1-m*m));
	}
	Vector axis = u*v;
	float angle;
	m=magnitude(axis);
	if(m>1)m=1;
	Quaternion q(Vector(1.0f,0.0f,0.0f),0.0f);
	if(m>0 && (angle=(float)asin(m))>3.14/180) {
 			axis = normalize(axis);
  			q=Quaternion(axis,angle);
	}
	return q;
}

void SpinIt(){
	Quaternion q=VirtualTrackBall(Vector(0,0,0),model_position,
	                              OldMouseVector,MouseVector);
	model_orientation=q*model_orientation;
}


/************************Mouse Interrupts******************************/
void mouseFunc(int button, int state, int x, int y)
{
	switch (button) 
	{
	case 2:
		if(state == GLUT_DOWN){
			mouseButtonPressed[0] = true;
			MouseX = x;
			MouseY = y;
			ComputeMouseVector();
			MouseState = 1;
		}
		else {
			mouseButtonPressed[0] = false;
			MouseX = x;
			MouseY = y;
			if(MouseX & 1 << 15) MouseX -= (1 << 16);
			if(MouseY & 1 << 15) MouseY -= (1 << 16);
			ComputeMouseVector();
			if(MouseState) SpinIt();
			MouseState=0;
		}
		break;
	case 0:
		if(state == GLUT_DOWN)
			mouseButtonPressed[2] = true;
		else
			mouseButtonPressed[2] = false;
		break;
	case 3:	
		break;
	case 4:
		break;
	}
}

void mouseDrag(int x, int y)
{
	if(mouseButtonPressed[0])	
	{
		MouseX = x;
		MouseY = y;
	    if(MouseX & 1 << 15) MouseX -= (1 << 16);
	    if(MouseY & 1 << 15) MouseY -= (1 << 16);
		ComputeMouseVector();
		if(MouseState) SpinIt();
		glutPostRedisplay();
	}
	if(mouseButtonPressed[2])
	{
		glutPostRedisplay();
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);			//2. Ask GLUT to create next windows with a RGB framebuffer and a Z-buffer too
	glutInitWindowSize(600,600);										//3. Tell GLUT how large are the windows we want to create next
	glutCreateWindow("Assignment 1: Texture Mapping Progressive Meshes");
	InitModel();
	glutKeyboardFunc(KeyDown); 
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseDrag);
	glutDisplayFunc(Display);									//5. Add a drawing callback to the window	
	glutReshapeFunc(Reshape);							//6. Add a resize callback to the window
	
	glutMainLoop();												//7. Start the event loop that displays the graph and handles window-resize events
   
	return 0;
}
