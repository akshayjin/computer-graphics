#include "stdio.h"
#include "math.h"
#include "GL/glut.h"
#include <iostream>
#include <vector>
#include "include/DataHandler.h"
#include "include/camera.h"
#include "include/Point3d.h"
#include "include/Quad.h"

// Decide number of evenly spaced isosurfaces
#define NUM_ISOSURFACE 4

// Orientation of Slice Plane
#define XY 0
#define YZ 1
#define ZX 2

using namespace std;

struct GLvector
{
	GLfloat fX;
	GLfloat fY;
	GLfloat fZ;     
};

// Slice Plane
struct Slice
{
	GLvector  vertices[4];
	int orientation = XY;
	float xAxis;
	float yAxis;
	float zAxis;
};

float isovalues[NUM_ISOSURFACE+1];
int isoNdx = 0;

/*******Perspective projection parameters and Window Size************/

float fov = 90;										
float aspect = 1; 
float z_near = .1; 
float z_far  = 140;
float scale_x = .5;
float scale_y = 1;
float scale_z = 1;		

int screenWidth = 1280;
int screenHeight = 768;
CCamera Camera;
/*********************************************************************/

//a2fVertexOffset lists the positions, relative to vertex0, of each of the 8 vertices of a cube
static const GLfloat a2fVertexOffset[8][3] =
{
        {0.0, 0.0, 0.0},{1.0, 0.0, 0.0},{1.0, 1.0, 0.0},{0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},{1.0, 0.0, 1.0},{1.0, 1.0, 1.0},{0.0, 1.0, 1.0}
};

//a2iEdgeConnection lists the index of the endpoint vertices for each of the 12 edges of the cube
static const GLint a2iEdgeConnection[12][2] = 
{
        {0,1}, {1,2}, {2,3}, {3,0},
        {4,5}, {5,6}, {6,7}, {7,4},
        {0,4}, {1,5}, {2,6}, {3,7}
};

//a2fEdgeDirection lists the direction vector (vertex1-vertex0) for each edge in the cube
static const GLfloat a2fEdgeDirection[12][3] =
{
        {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
        {1.0, 0.0, 0.0},{0.0, 1.0, 0.0},{-1.0, 0.0, 0.0},{0.0, -1.0, 0.0},
        {0.0, 0.0, 1.0},{0.0, 0.0, 1.0},{ 0.0, 0.0, 1.0},{0.0,  0.0, 1.0}
};

GLenum    ePolygonMode = GL_FILL;
GLfloat   fStepSize = 1.0;
GLfloat   fTargetValue;
Slice slice;
GLboolean bLight = false;		// Toggle Shading

vector <float> xvalues;
vector <float> yvalues;
vector <float> zvalues;

float grid[90][60][24];			// stores salintity values in the 3D grid


/******************forwar declarations*****************************/
void vDrawScene(); 
void vResize(GLsizei, GLsizei);
void vKeyboard(unsigned char cKey, int iX, int iY);
void vSpecial(int iKey, int iX, int iY);
void mouseFunc(int button, int state, int x, int y);
void mouseDrag(int x, int y);
bool mouseButtonPressed[5];
void slicingXY();
void slicingYZ();
void slicingZX();
void colormap();
GLvoid vPrintHelp();
GLvoid vMarchingCubes();
GLvoid vMarchCube(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale,int ndxX,int ndxY, int ndxZ);
void init();
/********************************************************************/
void init()
{
	slice.xAxis = (xvalues[(xvalues.size()-1)] + xvalues[0])/2;
	slice.yAxis = (yvalues[(yvalues.size()-1)] + yvalues[0])/2;
	slice.zAxis = (zvalues[(zvalues.size()-1)] + zvalues[0])/40;
}

int main(int argc, char **argv) 
{ 
				if(argc<2){
					cout << "Usage: ./assignment3 .csv" << endl;
					exit(-1);
				}
				DataHandler dh(argv[1]);
				dh.parseData();
				xvalues = dh.xvalues;
				yvalues = dh.yvalues;
				zvalues = dh.zvalues;
				int i=0;
				float min=100,max=0;
				cout << dh.XAxis << " " << dh.YAxis << " " << dh.ZAxis << endl;
				for(int z=0;z<zvalues.size();z++){
					for(int y=0;y<yvalues.size();y++){
						for(int x=0;x<xvalues.size();x++){
							grid[x][y][z] = dh.salinity[i];
							i++;
							if(dh.salinity[i]>max)
								max=dh.salinity[i];
							if(dh.salinity[i]<min && dh.salinity[i]!=0)
								min=dh.salinity[i];
						}
					}
				}
				
				for(int i=0;i<=NUM_ISOSURFACE;i++){
					isovalues[i] = min + i*(max-min)/NUM_ISOSURFACE;
				}
				
        GLfloat afPropertiesAmbient [] = {0.50, 0.50, 0.50, 1.00}; 
        GLfloat afPropertiesDiffuse [] = {0.75, 0.75, 0.75, 1.00}; 
        GLfloat afPropertiesSpecular[] = {1.00, 1.00, 1.00, 1.00}; 

        GLsizei iWidth = 640.0; 
        GLsizei iHeight = 480.0; 
				
				Camera.init(0,0,1,-1,0,0,0,1,0,87,0,-42,0,0,0);
				
				init();
        glutInit(&argc, argv);
        glutInitWindowPosition( 0, 0);
        glutInitWindowSize(iWidth, iHeight);
        glutInitDisplayMode( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
        glutCreateWindow( "Marching Cubes" );
        glutDisplayFunc( vDrawScene );
        glutReshapeFunc( vResize );
        glutKeyboardFunc( vKeyboard );
        glutSpecialFunc( vSpecial );
        glutMouseFunc(mouseFunc);
				glutMotionFunc(mouseDrag);

        glClearColor( 0.0, 0.0, 0.0, 1.0 ); 
        glClearDepth( 1.0 ); 

        glEnable(GL_DEPTH_TEST); 
        glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
        
				glShadeModel(GL_SMOOTH);							//2. Set shading parameters: use flat shading
				float light[4] = {1,1,1,0};							//3. Enable and set one directional light
				glEnable(GL_LIGHTING);
				glEnable(GL_LIGHT0);
				glLightfv(GL_LIGHT0,GL_POSITION,light);
				glEnable(GL_COLOR_MATERIAL);	

        vResize(iWidth, iHeight); 

        vPrintHelp();
        glutMainLoop(); 
        return 0;
}

GLvoid vPrintHelp()
{
        printf("Marching Cubes: Indian Ocean Salinty Data");

        printf("Controls:\n");
        printf("l: \t Toggle lighting\n");
        printf("l: \t Toggle slice plane orientation\n");
        printf("w,s: \t rotate y-axis\n");
        printf("a,d: \t rotate x-axis\n");
        printf("mouse click: \t rotate z-axis\n");
        printf("mouse wheel: \t zoom-in/out\n");
}


void vResize( GLsizei iWidth, GLsizei iHeight ) 
{ 
		screenHeight=iHeight;
		screenWidth=iWidth;
		glViewport( 0, 0, iWidth, iHeight ); 
		glEnable( GL_DEPTH_TEST );
		
		
		glMatrixMode (GL_PROJECTION);									//2. Set the projection matrix
		glLoadIdentity ();
		gluPerspective(fov,float(iWidth)/iHeight,z_near,z_far);
		
		glMatrixMode(GL_MODELVIEW);										//1. Set the modelview matrix (including the camera position and view direction)
		glLoadIdentity ();
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset( .5 , .5 );
}

/***********Keyboard Interrupt*****************/

void vKeyboard(unsigned char cKey, int iX, int iY)
{
	switch(cKey)
	{
		case 'a':		
		{
			Camera.RotateY(5.0);
			Camera.Print();
			break;
		}
		case 'd':		
		{
			Camera.RotateY(-5.0);
			Camera.Print();
			break;
		}
		case 'w':		
		{
			Camera.RotateX(5.0);
			Camera.Print();
			break;
		}
		case 's':		
		{
			Camera.RotateX(-5.0);
			Camera.Print();
			break;
		}

		case 'l' :
		{
			if(bLight)
			{
				glDisable(GL_LIGHTING);
			}
			else
			{
				glEnable(GL_LIGHTING);
			}

			bLight = !bLight;
		}break;
	}
	glutPostRedisplay();
}


void vSpecial(int iKey, int iX, int iY)
{
	switch(iKey)
	{
		case GLUT_KEY_PAGE_UP :
		{
			if(slice.orientation == XY)
				slice.zAxis = (slice.zAxis<=zvalues[0]/20) ? zvalues[0]/20 : (slice.zAxis-1);
			else if(slice.orientation == YZ)
				slice.xAxis = (slice.xAxis<=xvalues[0]) ? xvalues[0] : (slice.xAxis-1);
			else if(slice.orientation == ZX)
				slice.yAxis = (slice.yAxis<=yvalues[0]) ? yvalues[0] : (slice.yAxis-1);
		} break;
		case GLUT_KEY_PAGE_DOWN :
		{
			if(slice.orientation == XY)
				slice.zAxis = (slice.zAxis>=zvalues[zvalues.size()-1]/20) ? zvalues[zvalues.size()-1]/20 : (slice.zAxis+1);
			else if(slice.orientation == YZ)
				slice.xAxis = (slice.xAxis>=xvalues[xvalues.size()-1]) ? xvalues[xvalues.size()-1] : (slice.xAxis+1);
			else if(slice.orientation == ZX)
				slice.yAxis = (slice.yAxis>=yvalues[yvalues.size()-1]) ? yvalues[yvalues.size()-1] : (slice.yAxis+1);
		} break;
		case GLUT_KEY_HOME :
		{
			slice.orientation = (slice.orientation+1)%4;
		} break;
		case GLUT_KEY_RIGHT:
		{
			Camera.StrafeRight(1);
			break;
		}
		case GLUT_KEY_LEFT:
		{
			Camera.StrafeRight(-1);
			break;
		}
		case GLUT_KEY_UP:
		{
			Camera.MoveUpward(0.3);
			break;
		}
		case GLUT_KEY_DOWN:
		{
			Camera.MoveUpward(-0.3);
			break;
		}
	}
		glutPostRedisplay();
}

/************************Mouse Interrupts******************************/
void mouseFunc(int button, int state, int x, int y)
{
	switch (button) 
	{
	case 2:
		if(state == GLUT_DOWN)
			mouseButtonPressed[0] = true;
		else
			mouseButtonPressed[0] = false;
		break;
	case 0:
		if(state == GLUT_DOWN)
			mouseButtonPressed[2] = true;
		else
			mouseButtonPressed[2] = false;
		break;
	case 3:	
		Camera.MoveForward( -1 ) ;
		glutPostRedisplay();
		break;
	case 4:
		Camera.MoveForward( 1 ) ;
		glutPostRedisplay();
		break;
	}
}

void mouseDrag(int x, int y)
{
	if(mouseButtonPressed[0])
	{
		Camera.RotateZ(-.2);
		glutPostRedisplay();
	}
	if(mouseButtonPressed[2])
	{
		Camera.RotateZ(.2);
		glutPostRedisplay();
	}
}
/***********************************************************************/

// Draws a cuboid around the grid
void vBoundary()
{
	int lX = xvalues.size()-1;
	int lY = yvalues.size()-1;
	int lZ = zvalues.size()-1;
	
	glColor3f(1.0, 1.0, 1.0);
	glVertex3f(xvalues[0],yvalues[0],zvalues[0]/20);
	glVertex3f(xvalues[lX],yvalues[0],zvalues[0]/20);
	glVertex3f(xvalues[lX],yvalues[0],zvalues[lZ]/20);
	glVertex3f(xvalues[0],yvalues[0],zvalues[lZ]/20);
	
	glVertex3f(xvalues[0],yvalues[0],zvalues[0]/20);
	glVertex3f(xvalues[0],yvalues[lY],zvalues[0]/20);
	glVertex3f(xvalues[0],yvalues[lY],zvalues[lZ]/20);
	glVertex3f(xvalues[0],yvalues[0],zvalues[lZ]/20);
	
	glVertex3f(xvalues[lX],yvalues[0],zvalues[0]/20);
	glVertex3f(xvalues[lX],yvalues[lY],zvalues[0]/20);
	glVertex3f(xvalues[lX],yvalues[lY],zvalues[lZ]/20);
	glVertex3f(xvalues[lX],yvalues[0],zvalues[lZ]/20);
	
	glVertex3f(xvalues[lX],yvalues[lY],zvalues[0]/20);
	glVertex3f(xvalues[0],yvalues[lY],zvalues[0]/20);
	glVertex3f(xvalues[0],yvalues[lY],zvalues[lZ]/20);
	glVertex3f(xvalues[lX],yvalues[lY],zvalues[lZ]/20);
}

/*****************Slicing Functions w.r.t Orientation************************/
void slicingXY()
{
	 slice.vertices[0] = {xvalues[0]-10,yvalues[0]-10,slice.zAxis};
	 slice.vertices[1] = {xvalues[xvalues.size()-1]+10,yvalues[0]-10,slice.zAxis};
	 slice.vertices[2] = {xvalues[xvalues.size()-1]+10,yvalues[yvalues.size()-1]+10,slice.zAxis};
	 slice.vertices[3] = {xvalues[0]-10,yvalues[yvalues.size()-1]+10,slice.zAxis};
	 
	 glColor4f(1.0, 1.0, 0.0, .1);
	 glVertex3f(slice.vertices[0].fX,slice.vertices[0].fY,slice.vertices[0].fZ);
	 glVertex3f(slice.vertices[1].fX,slice.vertices[1].fY,slice.vertices[1].fZ);
	 glVertex3f(slice.vertices[2].fX,slice.vertices[2].fY,slice.vertices[2].fZ);
	 glVertex3f(slice.vertices[3].fX,slice.vertices[3].fY,slice.vertices[3].fZ);
}

void slicingYZ()
{
	 slice.vertices[0] = {slice.xAxis,yvalues[0]-10,zvalues[0]-10};
	 slice.vertices[1] = {slice.xAxis,yvalues[yvalues.size()-1]+10,zvalues[0]-10};
	 slice.vertices[2] = {slice.xAxis,yvalues[yvalues.size()-1]+10,zvalues[zvalues.size()-1]+10};
	 slice.vertices[3] = {slice.xAxis,yvalues[0]-10,zvalues[zvalues.size()-1]+10};
	 
	 glColor4f(1.0, 1.0, 0.0, .1);
	 glVertex3f(slice.vertices[0].fX,slice.vertices[0].fY,slice.vertices[0].fZ);
	 glVertex3f(slice.vertices[1].fX,slice.vertices[1].fY,slice.vertices[1].fZ);
	 glVertex3f(slice.vertices[2].fX,slice.vertices[2].fY,slice.vertices[2].fZ);
	 glVertex3f(slice.vertices[3].fX,slice.vertices[3].fY,slice.vertices[3].fZ);
}

void slicingZX()
{
	 slice.vertices[0] = {xvalues[0]-10,slice.yAxis,zvalues[0]-10};
	 slice.vertices[1] = {xvalues[xvalues.size()-1]+10,slice.yAxis,zvalues[0]-10};
	 slice.vertices[2] = {xvalues[xvalues.size()-1]+10,slice.yAxis,zvalues[zvalues.size()-1]+10};
	 slice.vertices[3] = {xvalues[0]-10,slice.yAxis,zvalues[zvalues.size()-1]+10};
	 
	 glColor4f(1.0, 1.0, 0.0, .1);
	 glVertex3f(slice.vertices[0].fX,slice.vertices[0].fY,slice.vertices[0].fZ);
	 glVertex3f(slice.vertices[1].fX,slice.vertices[1].fY,slice.vertices[1].fZ);
	 glVertex3f(slice.vertices[2].fX,slice.vertices[2].fY,slice.vertices[2].fZ);
	 glVertex3f(slice.vertices[3].fX,slice.vertices[3].fY,slice.vertices[3].fZ);
}
/*************************************************************************************/

void vDrawScene() 
{ 
		glViewport(0,0,screenWidth*.75,screenHeight);
		
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
		
		glMatrixMode(GL_MODELVIEW);							//1. Set the modelview matrix (including the camera position and view direction)
		glLoadIdentity ();
		glScalef(scale_x,scale_y,scale_z);
		Camera.Render();
		
		glPushMatrix(); 

		glPushMatrix(); 
		glTranslatef(-0.5, -0.5, -0.5);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBegin( GL_QUADS );
						vBoundary();
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
		
		glEnable(GL_BLEND);									//when using thick lines
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
		
		for(int i=0;i<NUM_ISOSURFACE;i++){
			fTargetValue = isovalues[i];
			isoNdx = i;
			glBegin(GL_TRIANGLES);
						vMarchingCubes();
			glEnd();
		}
		
		ePolygonMode = GL_LINE;
		glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
		for(int i=0;i<NUM_ISOSURFACE;i++){
			fTargetValue = isovalues[i];
			isoNdx = i;
			//~ glBegin(GL_TRIANGLES);
						//~ vMarchingCubes();
			//~ glEnd();
		}
		ePolygonMode = GL_FILL;
		
		/**************************************************/
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glBegin( GL_QUADS );
			if(slice.orientation == XY)
				slicingXY();
			else if(slice.orientation == YZ)
				slicingYZ();
			else if(slice.orientation == ZX)
				slicingZX();
		glEnd();
		glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
		/**************************************************/
		
		/**************************************************/
		glPushMatrix(); 
		glMatrixMode(GL_MODELVIEW);										//1. Set the modelview matrix (including the camera position and view direction)
		glLoadIdentity ();
		if(slice.orientation == XY)	
			gluLookAt(74,0,50, 74.1,0,1, 0,1,0);
		else if(slice.orientation == YZ)	
			gluLookAt(70,0,50, 1,0,50.1, 0,0,1);
		else if(slice.orientation == ZX)	
			gluLookAt(50,74,60, 50.1,75,1, 0,1,0);
		glViewport(screenWidth*.75,0,screenWidth*.25,screenHeight);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDisable(GL_LIGHTING);
		glBegin( GL_QUADS );
			colormap();
		glEnd();
		if(bLight==true)
			glEnable(GL_LIGHTING);
		glScalef(.75,.75,.75);
		glPolygonMode(GL_FRONT_AND_BACK, ePolygonMode);
		glPopMatrix();
		/***************************************************/
		
		glPopMatrix(); 
		glPopMatrix(); 

		glutSwapBuffers(); 
}

//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
GLfloat fGetOffset(GLfloat fValue1, GLfloat fValue2, GLfloat fValueDesired)
{
        GLdouble fDelta = fValue2 - fValue1;

        if(fDelta == 0.0)
        {
                return 0.5;
        }
        return (fValueDesired - fValue1)/fDelta;
}

float isoColors[5][3] = {{1,0,0},{1,1,0},{1,0.4,0.8},{0,.8,0},{0,.2,.8}};
//vGetColor generates a color from a given isovalue and returns transperancy
GLfloat vGetColor(GLvector &rfColor, GLvector &rfPosition, GLvector &rfNormal)
{
        GLfloat fX = rfNormal.fX;
        GLfloat fY = rfNormal.fY;
        GLfloat fZ = rfNormal.fZ;

        if(ePolygonMode==GL_LINE){
					rfColor.fX = 1;
					rfColor.fY = 1;
					rfColor.fZ = 1;
					return 1;
				}
				else{
					rfColor.fX = isoColors[isoNdx][0];
					rfColor.fY = isoColors[isoNdx][1];
					rfColor.fZ = isoColors[isoNdx][2];
					return 1;
				}
        
}

GLvoid vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource)
{
        GLfloat fOldLength;
        GLfloat fScale;

        fOldLength = sqrtf( (rfVectorSource.fX * rfVectorSource.fX) +
                            (rfVectorSource.fY * rfVectorSource.fY) +
                            (rfVectorSource.fZ * rfVectorSource.fZ) );

        if(fOldLength == 0.0)
        {
                rfVectorResult.fX = rfVectorSource.fX;
                rfVectorResult.fY = rfVectorSource.fY;
                rfVectorResult.fZ = rfVectorSource.fZ;
        }
        else
        {
                fScale = 1.0/fOldLength;
                rfVectorResult.fX = rfVectorSource.fX*fScale;
                rfVectorResult.fY = rfVectorSource.fY*fScale;
                rfVectorResult.fZ = rfVectorSource.fZ*fScale;
        }
}

//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
GLvoid vGetNormal(GLvector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ,int ndxX,int ndxY, int ndxZ)
{
        rfNormal.fX = grid[ndxX-1][ndxY][ndxZ] - grid[ndxX+1][ndxY][ndxZ];
        rfNormal.fY = grid[ndxX][ndxY-1][ndxZ] - grid[ndxX][ndxY+1][ndxZ];
        rfNormal.fZ = grid[ndxX][ndxY][ndxZ-1] - grid[ndxX][ndxY][ndxZ+1];
        vNormalizeVector(rfNormal, rfNormal);
}


//vMarchCube performs the Marching Cubes algorithm on a single cube
GLvoid vMarchCube(GLfloat fX, GLfloat fY, GLfloat fZ, GLfloat fScale,int ndxX,int ndxY, int ndxZ)
{
        extern GLint aiCubeEdgeFlags[256];
        extern GLint a2iTriangleConnectionTable[256][16];

        GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        GLfloat fOffset;
        GLvector sColor;
        GLfloat afCubeValue[8];
        GLvector asEdgeVertex[12];
        GLvector asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        afCubeValue[0] = grid[ndxX][ndxY][ndxZ];
        afCubeValue[1] = grid[ndxX+1][ndxY][ndxZ];
        afCubeValue[2] = grid[ndxX+1][ndxY+1][ndxZ];
        afCubeValue[3] = grid[ndxX][ndxY+1][ndxZ];
        afCubeValue[4] = grid[ndxX][ndxY][ndxZ+1];
        afCubeValue[5] = grid[ndxX+1][ndxY][ndxZ+1];
        afCubeValue[6] = grid[ndxX+1][ndxY+1][ndxZ+1];
        afCubeValue[7] = grid[ndxX][ndxY+1][ndxZ+1];

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= fTargetValue) 
                        iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0) 
        {
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
					//if there is an intersection on this edge
					if(iEdgeFlags & (1<<iEdge))
					{
						fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], 
																				 afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTargetValue);

						asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
						asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
						asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2] * (zvalues[ndxZ+1]-zvalues[ndxZ])/20  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

						vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ,ndxX,ndxY,ndxZ);
					}
        }


        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
					if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0)
						break;

					for(iCorner = 0; iCorner < 3; iCorner++)
					{
						iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

						float alpha = vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
						glColor4f(sColor.fX, sColor.fY, sColor.fZ, alpha);
						glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
						glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
					}
        }
}
        

//vMarchingCubes iterates over the entire dataset, calling vMarchCube on each cube
GLvoid vMarchingCubes()
{
        GLint iX, iY, iZ;
        for(iX = 0; iX < xvalues.size()-1; iX++)
        for(iY = 0; iY < yvalues.size()-1; iY++)
        for(iZ = 0; iZ < zvalues.size()-1; iZ++)
        {
					vMarchCube(xvalues[iX], yvalues[iY], zvalues[iZ]/20, fStepSize, iX,iY,iZ);
        }
}

// Colormap for Sliced plane, conditionals to separate orientation
void colormap() 
{

	int sal_index  = 0 ;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Quad q;

	float R,G,B;
	if(slice.orientation==XY)
	{
		// Visit every grid and add color to the quadrilateral
		int YDim = yvalues.size();
		int XDim = xvalues.size();
		int z = slice.zAxis;
		float min=100,max=0;
		for (int y = 0 ; y < YDim-1 ; y++) 
		{
			for (int x = 0 ; x < XDim-1; x++)
			{
				if(grid[x][y][z]>max)
					max=grid[x][y][z];
				if(grid[x][y][z]<min && grid[x][y][z]!=0)
					min = grid[x][y][z];
			}
		}
		
		for (int y = 0 ; y < YDim-1 ; y++) 
		{
			for (int x = 0 ; x < XDim-1; x++)
			{
				float normalised = (grid[x][y][z] - min)/(max-min);
				if (grid[x][y][z]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p0 (xvalues[x],yvalues[y],0);						// scale normalised salinity to Z index
				q.addPoint(p0.x,p0.y,p0.z);

				normalised = (grid[x+1][y][z] - min)/(max-min);
				if (grid[x+1][y][z]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p1 (xvalues[x+1],yvalues[y],0);
				q.addPoint(p1.x,p1.y,p1.z);

				normalised = (grid[x+1][y+1][z] - min)/(max-min);
				if (grid[x+1][y+1][z]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p2 (xvalues[x+1],yvalues[y+1],0);
				q.addPoint(p2.x,p2.y,p2.z);

				normalised = (grid[x][y+1][z] - min)/(max-min);
				if (grid[x][y+1][z]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p3 (xvalues[x],yvalues[y+1],0);
				q.addPoint(p3.x,p3.y,p3.z);
			}
		}
		q.draw();
		
		Quad legend;
		glColor3f(0,0,0) ;
		Point3d l0 (xvalues[0],yvalues[0]-5,0);
		legend.addPoint(l0.x,l0.y,l0.z);
		glColor3f(0,0,0) ;
		Point3d l1 (xvalues[0],yvalues[0]-10,0);
		legend.addPoint(l1.x,l1.y,l1.z);
		glColor3f(1,1,1) ;
		Point3d l2 (xvalues[XDim-1],yvalues[0]-10,0);
		legend.addPoint(l2.x,l2.y,l2.z);
		glColor3f(1,1,1) ;
		Point3d l3 (xvalues[XDim-1],yvalues[0]-5,0);
		legend.addPoint(l3.x,l3.y,l3.z);
		legend.draw();
		
		glColor3f(1,0,0) ;
		char val[4];
		sprintf(val, "%.2f", min);
		glRasterPos2f( xvalues[0],yvalues[0]-10); // location to start printing text
		for( int digit=0; digit < 4; digit++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
		sprintf(val, "%.2f", max);
		glRasterPos3i( xvalues[XDim-1],yvalues[0]-10,0); // location to start printing text
		for( int digit=0; digit < 4; digit++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
		
		float sector = (xvalues[XDim-1] - xvalues[0])/NUM_ISOSURFACE;
		int j = xvalues[0];
		for(int i=0;i<NUM_ISOSURFACE;i++){
			Quad legend;
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l0 (j,yvalues[0]-12,0);
			legend.addPoint(l0.x,l0.y,l0.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l1 (j,yvalues[0]-17,0);
			legend.addPoint(l1.x,l1.y,l1.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l2 (j+sector,yvalues[0]-17,0);
			legend.addPoint(l2.x,l2.y,l2.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l3 (j+sector,yvalues[0]-12,0);
			legend.addPoint(l3.x,l3.y,l3.z);
			legend.draw();
			
			char val[4];
			sprintf(val, "%.2f", isovalues[i]);
			glColor3f(1,1,1);
			glRasterPos3i( j,yvalues[0]-17,0); // location to start printing text
			for( int digit=0; digit < 4; digit++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
			j+=sector;
		}
		
	}
	else if(slice.orientation==YZ)
	{
		// Visit every grid and add color to the quadrilateral
		int YDim = yvalues.size();
		int ZDim = zvalues.size();
		int x = slice.xAxis;
		if(x>=xvalues.size())
			x=xvalues.size()-1;
		if(x<0)
			x=0;
		
		float min=100,max=0;
		
		for (int z = 0 ; z < ZDim-1 ; z++) 
		{
			for (int y = 0 ; y < YDim-1; y++)
			{
				if(grid[x][y][z]>max)
					max=grid[x][y][z];
				if(grid[x][y][z]<min && grid[x][y][z]!=0)
					min = grid[x][y][z];
			}
		}
		
		for (int z = 0 ; z < ZDim-1 ; z++) 
		{
			for (int y = 0 ; y < YDim-1; y++)
			{
				float normalised = (grid[x][y][z] - min)/(max-min);
				if (grid[x][y][z]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p0 (0,yvalues[y],zvalues[z]/20);						// scale normalised salinity to Z index
				q.addPoint(p0.x,p0.y,p0.z);

				normalised = (grid[x][y+1][z] - min)/(max-min);
				if (grid[x][y+1][z]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p1 (0,yvalues[y+1],zvalues[z]/20);
				q.addPoint(p1.x,p1.y,p1.z);

				normalised = (grid[x][y+1][z+1] - min)/(max-min);
				if (grid[x][y+1][z+1]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p2 (0,yvalues[y+1],zvalues[z+1]/20);
				q.addPoint(p2.x,p2.y,p2.z);

				normalised = (grid[x][y][z+1] - min)/(max-min);
				if (grid[x][y][z+1]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p3 (0,yvalues[y],zvalues[z+1]/20);
				q.addPoint(p3.x,p3.y,p3.z);
			}
		}
		q.draw();
		
		Quad legend;
		glColor3f(0,0,0) ;
		Point3d l0 (0,yvalues[0],zvalues[0]-7);
		legend.addPoint(l0.x,l0.y,l0.z);
		glColor3f(0,0,0) ;
		Point3d l1 (0,yvalues[0],zvalues[0]-12);
		legend.addPoint(l1.x,l1.y,l1.z);
		glColor3f(1,1,1) ;
		Point3d l2 (0,yvalues[YDim-1],zvalues[0]-12);
		legend.addPoint(l2.x,l2.y,l2.z);
		glColor3f(1,1,1) ;
		Point3d l3 (0,yvalues[YDim-1],zvalues[0]-7);
		legend.addPoint(l3.x,l3.y,l3.z);
		legend.draw();
		
		glColor3f(1,0,0) ;
		char val[4];
		sprintf(val, "%.2f", min);
		glRasterPos3i( 0,yvalues[0],zvalues[0]-12); // location to start printing text
		for( int digit=0; digit < 4; digit++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
		sprintf(val, "%.2f", max);
		glRasterPos3i( 0,yvalues[YDim-1],zvalues[0]-12); // location to start printing text
		for( int digit=0; digit < 4; digit++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
		
		float sector = (yvalues[YDim-1] - yvalues[0])/NUM_ISOSURFACE;
		int j = yvalues[0];
		for(int i=0;i<NUM_ISOSURFACE;i++){
			Quad legend;
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l0 (0,j,zvalues[0]-14);
			legend.addPoint(l0.x,l0.y,l0.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l1 (0,j,zvalues[0]-19);
			legend.addPoint(l1.x,l1.y,l1.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l2 (0,j+sector,zvalues[0]-19);
			legend.addPoint(l2.x,l2.y,l2.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l3 (0,j+sector,zvalues[0]-14);
			legend.addPoint(l3.x,l3.y,l3.z);
			legend.draw();
			
			char val[4];
			sprintf(val, "%.2f", isovalues[i]);
			glColor3f(1,1,1);
			glRasterPos3i( 0,j,zvalues[0]-18); // location to start printing text
			for( int digit=0; digit < 4; digit++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
			j+=sector;
		}
	}
	else if(slice.orientation==ZX)
	{
		// Visit every grid and add color to the quadrilateral
		vector <float> yvalues = xvalues;
		vector <float> xvalues = zvalues;
		for(int i=0;i<xvalues.size();i++)
			xvalues[i]/=20;
			
		int YDim = yvalues.size();
		int XDim = xvalues.size();
		int z = slice.yAxis;
		float min=100,max=0;
		for (int y = 0 ; y < YDim-1 ; y++) 
		{
			for (int x = 0 ; x < XDim-1; x++)
			{
				if(grid[y][z][x]>max)
					max=grid[y][z][x];
				if(grid[y][z][x]<min && grid[y][z][x]!=0)
					min = grid[y][z][x];
			}
		}
		
		for (int y = 0 ; y < YDim-1 ; y++) 
		{
			for (int x = 0 ; x < XDim-1; x++)
			{
				float normalised = (grid[y][z][x] - min)/(max-min);
				if (grid[y][z][x]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p0 (xvalues[x],yvalues[y],0);						// scale normalised salinity to Z index
				q.addPoint(p0.x,p0.y,p0.z);

				normalised = (grid[y][z][x+1] - min)/(max-min);
				if (grid[y][z][x+1]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p1 (xvalues[x+1],yvalues[y],0);
				q.addPoint(p1.x,p1.y,p1.z);

				normalised = (grid[y+1][z][x+1] - min)/(max-min);
				if (grid[y+1][z][x+1]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p2 (xvalues[x+1],yvalues[y+1],0);
				q.addPoint(p2.x,p2.y,p2.z);

				normalised = (grid[y+1][z][x] - min)/(max-min);
				if (grid[y+1][z][x]== 0) glColor3f(0,0,0);
				else glColor3f(normalised,normalised,normalised) ;
				Point3d p3 (xvalues[x],yvalues[y+1],0);
				q.addPoint(p3.x,p3.y,p3.z);
			}
		}
		q.draw();
		
		Quad legend;
		glColor3f(0,0,0) ;
		Point3d l0 (xvalues[0],yvalues[0],0);
		legend.addPoint(l0.x,l0.y,l0.z);
		glColor3f(0,0,0) ;
		Point3d l1 (xvalues[0],yvalues[0]-5,0);
		legend.addPoint(l1.x,l1.y,l1.z);
		glColor3f(1,1,1) ;
		Point3d l2 (xvalues[XDim-1],yvalues[0]-5,0);
		legend.addPoint(l2.x,l2.y,l2.z);
		glColor3f(1,1,1) ;
		Point3d l3 (xvalues[XDim-1],yvalues[0],0);
		legend.addPoint(l3.x,l3.y,l3.z);
		legend.draw();
		
		glColor3f(1,0,0) ;
		char val[4];
		sprintf(val, "%.2f", min);
		glRasterPos3i( xvalues[0],yvalues[0]-5,0); // location to start printing text
		for( int digit=0; digit < 4; digit++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
		sprintf(val, "%.2f", max);
		glRasterPos3i( xvalues[XDim-1],yvalues[0]-5,0); // location to start printing text
		for( int digit=0; digit < 4; digit++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
			
		float sector = (xvalues[XDim-1] - xvalues[0])/NUM_ISOSURFACE;
		int j = xvalues[0];
		for(int i=0;i<NUM_ISOSURFACE;i++){
			Quad legend;
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l0 (j,yvalues[0]-7,0);
			legend.addPoint(l0.x,l0.y,l0.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l1 (j,yvalues[0]-12,0);
			legend.addPoint(l1.x,l1.y,l1.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l2 (j+sector,yvalues[0]-12,0);
			legend.addPoint(l2.x,l2.y,l2.z);
			glColor3f(isoColors[i][0],isoColors[i][1],isoColors[i][2]) ;
			Point3d l3 (j+sector,yvalues[0]-7,0);
			legend.addPoint(l3.x,l3.y,l3.z);
			legend.draw();
			
			char val[4];
			sprintf(val, "%.2f", isovalues[i]);
			glColor3f(1,1,1);
			glRasterPos3i( j,yvalues[0]-12,0); // location to start printing text
			for( int digit=0; digit < 4; digit++)
				glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, val[digit] ); // Print a character on the screen
			j+=sector;
		}
	}
}


// For any edge, if one vertex is inside of the surface and the other is outside of the surface
//  then the edge intersects the surface
// For each of the 8 vertices of the cube can be two possible states : either inside or outside of the surface
// For any cube the are 2^8=256 possible sets of vertex states
// This table lists the edges intersected by the surface for all 256 possible vertex states
// There are 12 edges.  For each entry in the table, if edge #n is intersected, then bit #n is set to 1

GLint aiCubeEdgeFlags[256]=
{
        0x000, 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c, 0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00, 
        0x190, 0x099, 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c, 0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90, 
        0x230, 0x339, 0x033, 0x13a, 0x636, 0x73f, 0x435, 0x53c, 0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30, 
        0x3a0, 0x2a9, 0x1a3, 0x0aa, 0x7a6, 0x6af, 0x5a5, 0x4ac, 0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0, 
        0x460, 0x569, 0x663, 0x76a, 0x066, 0x16f, 0x265, 0x36c, 0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60, 
        0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0x0ff, 0x3f5, 0x2fc, 0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0, 
        0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x055, 0x15c, 0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950, 
        0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0x0cc, 0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0, 
        0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc, 0x0cc, 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0, 
        0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c, 0x15c, 0x055, 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650, 
        0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc, 0x2fc, 0x3f5, 0x0ff, 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0, 
        0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c, 0x36c, 0x265, 0x16f, 0x066, 0x76a, 0x663, 0x569, 0x460, 
        0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac, 0x4ac, 0x5a5, 0x6af, 0x7a6, 0x0aa, 0x1a3, 0x2a9, 0x3a0, 
        0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c, 0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x033, 0x339, 0x230, 
        0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c, 0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x099, 0x190, 
        0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c, 0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x000
};

//  For each of the possible vertex states listed in aiCubeEdgeFlags there is a specific triangulation
//  of the edge intersection points.  a2iTriangleConnectionTable lists all of them in the form of
//  0-5 edge triples with the list terminated by the invalid value -1.
//  For example: a2iTriangleConnectionTable[3] list the 2 triangles formed when corner[0] 
//  and corner[1] are inside of the surface, but the rest of the cube is not.
//
//  I found this table in an example program someone wrote long ago.  It was probably generated by hand

GLint a2iTriangleConnectionTable[256][16] =  
{
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
        {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
        {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
        {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
        {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
        {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
        {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
        {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
        {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
        {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
        {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
        {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
        {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
        {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
        {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
        {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
        {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
        {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
        {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
        {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
        {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
        {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
        {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
        {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
        {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
        {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
        {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
        {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
        {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
        {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
        {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
        {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
        {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
        {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
        {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
        {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
        {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
        {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
        {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
        {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
        {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
        {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
        {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
        {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
        {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
        {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
        {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
        {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
        {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
        {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
        {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
        {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
        {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
        {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
        {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
        {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
        {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
        {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
        {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
        {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
        {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
        {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
        {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
        {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
        {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
        {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
        {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
        {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
        {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
        {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
        {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
        {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
        {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
        {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
        {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
        {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
        {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
        {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
        {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
        {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
        {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
        {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
        {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
        {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
        {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
        {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
        {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
        {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
        {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
        {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
        {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
        {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
        {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
        {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
        {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
        {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
        {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
        {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
        {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
        {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
        {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
        {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
        {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
        {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
        {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
        {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
        {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
        {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
        {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
        {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
        {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
        {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
        {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
        {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
        {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
        {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
        {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
        {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
        {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
        {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
        {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
        {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
        {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
        {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
        {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
        {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
        {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
        {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};
