#include <GL/gl.h>	
#include "include/Model.h"
#include "include/View.h"
#include "include/Controller.h"
#include <iostream>
#include <GL/glut.h> 

using namespace std;

void Model::init()
{
	texture_files.push_back("Textures/checker.bmp");
	texture_files.push_back("Textures/grass.bmp");
	texture_files.push_back("Textures/skin.bmp");
	texture_files.push_back("Textures/rocky.bmp");
	texture_files.push_back("Textures/wood.bmp");
	texture_files.push_back("Textures/greenapple.bmp");
	texture_files.push_back("Textures/sawblade.bmp");
	for(int i=0;i<texture_files.size();i++)
	{
	 texture_ids.push_back(loadBMP_custom(texture_files[i].c_str()));
	}
	if (!sawblade) {
		char objectName[100] = {"assets/sawblade.obj"};
		sawblade = glmReadOBJ(objectName);
		if (!sawblade) exit(0);
		glmUnitize(sawblade);
		glmFacetNormals(sawblade);
		glmVertexNormals(sawblade, 90.0);
		glmScale(sawblade, 2.5);
	}
	if (!slab) {
		char objectName[100] = {"assets/slab.obj"};
		slab = glmReadOBJ(objectName);
		if (!slab) exit(0);
		glmUnitize(slab);
		glmFacetNormals(slab);
		glmVertexNormals(slab, 90.0);
		glmScale(slab, 2);
	}
	if (!cube) {
		char objectName[100] = {"assets/cube.obj"};
		cube = glmReadOBJ(objectName);
		if (!cube) exit(0);
		glmUnitize(cube);
		glmFacetNormals(cube);
		glmVertexNormals(cube, 90.0);
		glmScale(cube, 2.5);
	}
	if (!sphere) {
		char objectName[100] = {"assets/sphere.obj"};
		sphere = glmReadOBJ(objectName);
		if (!sphere) exit(0);
		glmUnitize(sphere);
		glmFacetNormals(sphere);
		glmVertexNormals(sphere, 90.0);
		glmScale(sphere, 3);
	}
	if (!cone) {
		char objectName[100] = {"assets/cone.obj"};
		cone = glmReadOBJ(objectName);
		if (!cone) exit(0);
		glmUnitize(cone);
		glmFacetNormals(cone);
		glmVertexNormals(cone, 90.0);
		glmScale(cone, 3);
	}
	if (!bunny) {
		char objectName[100] = {"assets/stanford_bunny.obj"};
		bunny = glmReadOBJ(objectName);
		if (!bunny) exit(0);
		glmUnitize(bunny);
		glmFacetNormals(bunny);
		glmVertexNormals(bunny, 90.0);
		glmScale(bunny, 4);
	}
	gCurrentEffect = EffectFactory::create("fountain");
	gCurrentEffect->initialize(START_NUM_PARTICLES);
	gCurrentEffect->initializeRenderer();
}

void Model::render()
{
	glTranslatef(transx/100,transz/10,transy/100);
	glPushMatrix();
		drawFloor();
	glPopMatrix();
	
	glPushMatrix();
		glRotatef(90,0,1,0);
		glTranslatef(1,2.5,0);
		glRotatef(saw_rotation,0,0,-1);
		drawSawBlade();
		saw_rotation = (saw_rotation+20)%360;
	glPopMatrix();
	
	glPushMatrix();
		glRotatef(90,0,1,0);
		glTranslatef(0,1.75,0);
		drawSlab();
	glPopMatrix();
	
	glPushMatrix();
		glTranslatef(0,2.1,-0.6);
		drawParticles();
	glPopMatrix();
	
	glPushMatrix();
		drawObjects();
	glPopMatrix();
	
	
	
	glPushMatrix();
		//~ glLoadIdentity ();
		glDisable(GL_LIGHTING);
		drawAxis();
	glPopMatrix();
}

void Model::drawParticles()
{
	gCurrentEffect->render();
	gCurrentEffect->update(DELTA_TIME);
	gCurrentEffect->cpuUpdate(DELTA_TIME);
}

void Model::drawObjects()
{
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ids[3]);
	glEnable(GL_TEXTURE_2D);
	// cube
	glPushMatrix();
		
		glTranslatef(0,0.4,3);
		glmDraw(cube, GLM_SMOOTH | GLM_TEXTURE);
		
	glPopMatrix();
	
	// sphere
	glPushMatrix();
		glTranslatef(1,0.65,2);
		glmDraw(sphere, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	
	// cone
	glPushMatrix();
		glTranslatef(1.5,0.5,3.5);
		glmDraw(cone, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	
	// bunny
	glPushMatrix();
		glTranslatef(-1,1.05,3.5);
		glmDraw(bunny, GLM_SMOOTH | GLM_TEXTURE);
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	
	
	/********************Bounding Boxes*****************************/
	/*
	// cube bounding box
	glColor3f(0,1,0);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
	glBegin(GL_QUADS);
		glVertex3f(-0.42,0,2.58);
		glVertex3f(-0.42,0.82,2.58);
		glVertex3f(0.42,0.82,2.58);
		glVertex3f(0.42,0,2.58);
		
		glVertex3f(-0.42,0,2.58);
		glVertex3f(0.42,0,2.58);
		glVertex3f(0.42,0,3.42);
		glVertex3f(-0.42,0,3.42);
		
		glVertex3f(-0.42,0.82,2.58);
		glVertex3f(0.42,0.82,2.58);
		glVertex3f(0.42,0.82,3.42);
		glVertex3f(-0.42,0.82,3.42);
		
		glVertex3f(-0.42,0,3.42);
		glVertex3f(-0.42,0.82,3.42);
		glVertex3f(0.42,0.82,3.42);
		glVertex3f(0.42,0,3.42);
	glEnd();
	
	// sphere bounding sphere
	glPushMatrix();
		glTranslatef(1,0.65,2);
		glutWireSphere(0.655,10,10);
	glPopMatrix();
	
	// cone bounding bounding
	glPushMatrix();
		glTranslatef(1.5,0.0,3.5);
		glRotatef(90,-1,0,0);
		GLUquadricObj *quadratic;
		quadratic = gluNewQuadric();
		gluCylinder(quadratic, 0.995f, 0.01f, 1, 12, 12);
	glPopMatrix();
	
	// bunny bounding boxes
	//body
	glBegin(GL_QUADS);
		glVertex3f(-1.55,0,2.58);
		glVertex3f(-1.55,1.3,2.58);
		glVertex3f(-0.4,1.3,2.58);
		glVertex3f(-0.4,0,2.58);
		
		glVertex3f(-1.55,0,2.58);
		glVertex3f(-0.4,0,2.58);
		glVertex3f(-0.4,0,4.0);
		glVertex3f(-1.55,0,4.0);
		
		glVertex3f(-1.55,1.3,2.58);
		glVertex3f(-0.4,1.3,2.58);
		glVertex3f(-0.4,1.3,4.0);
		glVertex3f(-1.55,1.3,4.0);
		
		glVertex3f(-1.55,0,4.0);
		glVertex3f(-1.55,1.3,4.0);
		glVertex3f(-0.4,1.3,4.0);
		glVertex3f(-0.4,0,4.0);
	glEnd();
	
	//head
		glBegin(GL_QUADS);
		glVertex3f(-1.0,1.3,3.7);
		glVertex3f(-1.0,1.65,3.7);
		glVertex3f(-0.2,1.65,3.7);
		glVertex3f(-0.2,1.3,3.7);
		
		glVertex3f(-1.0,1.3,3.7);
		glVertex3f(-0.2,1.3,3.7);
		glVertex3f(-0.2,1.3,4.3);
		glVertex3f(-1.0,1.3,4.3);
		
		glVertex3f(-1.0,1.65,3.7);
		glVertex3f(-0.2,1.65,3.7);
		glVertex3f(-0.2,1.65,4.3);
		glVertex3f(-1.0,1.65,4.3);
		
		glVertex3f(-1.0,1.3,4.3);
		glVertex3f(-1.0,1.65,4.3);
		glVertex3f(-0.2,1.65,4.3);
		glVertex3f(-0.2,1.3,4.3);
	
	//ear far
		glBegin(GL_QUADS);
		glVertex3f(-1.6,1.5,4.15);
		glVertex3f(-1.6,2.0,4.15);
		glVertex3f(-1.0,2.0,4.15);
		glVertex3f(-1.0,1.5,4.15);
		
		glVertex3f(-1.6,1.5,4.15);
		glVertex3f(-1.0,1.5,4.15);
		glVertex3f(-1.0,1.5,4.5);
		glVertex3f(-1.6,1.5,4.5);
		
		glVertex3f(-1.6,2.0,4.15);
		glVertex3f(-1.0,2.0,4.15);
		glVertex3f(-1.0,2.0,4.5);
		glVertex3f(-1.6,2.0,4.5);
		
		glVertex3f(-1.6,1.5,4.5);
		glVertex3f(-1.6,2.0,4.5);
		glVertex3f(-1.0,2.0,4.5);
		glVertex3f(-1.0,1.5,4.5);
	glEnd();
	
	//ear near
		glBegin(GL_QUADS);
		glVertex3f(-1.65,1.5,3.55);
		glVertex3f(-1.65,2.1,3.55);
		glVertex3f(-1.1,2.1,3.55);
		glVertex3f(-1.1,1.5,3.55);
		
		glVertex3f(-1.65,1.5,3.55);
		glVertex3f(-1.1,1.5,3.55);
		glVertex3f(-1.1,1.5,3.85);
		glVertex3f(-1.65,1.5,3.85);
		
		glVertex3f(-1.65,2.1,3.55);
		glVertex3f(-1.1,2.1,3.55);
		glVertex3f(-1.1,2.1,3.85);
		glVertex3f(-1.65,2.1,3.85);
		
		glVertex3f(-1.65,1.5,3.85);
		glVertex3f(-1.65,2.1,3.85);
		glVertex3f(-1.1,2.1,3.85);
		glVertex3f(-1.1,1.5,3.85);
	glEnd();
	glColor3f(1,1,1);
	*/
	
}

void Model::drawFloor()
{
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ids[4]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
		glTexCoord2f(0,0);glVertex3f(-6,0,-6);
		glTexCoord2f(0,1);glVertex3f(-6,0,6);
		glTexCoord2f(1,1);glVertex3f(6,0,6);
		glTexCoord2f(1,0);glVertex3f(6,0,-6);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void Model::drawSawBlade()
{
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ids[6]);
    glEnable(GL_TEXTURE_2D);
	glmDraw(sawblade, GLM_SMOOTH | GLM_TEXTURE);
	glDisable(GL_TEXTURE_2D);
}

void Model::drawSlab()
{
	glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_ids[2]);
    glEnable(GL_TEXTURE_2D);
	glmDraw(slab, GLM_SMOOTH | GLM_TEXTURE);
	glDisable(GL_TEXTURE_2D);
}

void Model::drawAxis()
{
	glBegin(GL_LINES);
		glColor3f(1,0,0);
		glVertex3f(-1000,0,0);
		glVertex3f(1000,0,0);
		glColor3f(0,1,0);
		glVertex3f(0,-1000,0);
		glVertex3f(0,1000,0);
		glColor3f(0,0,1);
		glVertex3f(0,0,-1000);
		glVertex3f(0,0,1000);
	glEnd();
}

void Model::rotate(float deg)
{
}

void Model::scale(float times)
{
	glScalef(times,times,times);
}

void Model::translate(float x, float y, float z)
{
	transx = x; transy = -y; transz = z;
}

GLuint Model::loadBMP_custom(const char * imagepath){
    // Read .BMP File
    // Data read from the header of the BMP file
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;

    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file){printf("Image could not be opened\n"); return 0;}
    if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
        printf("Not a correct BMP file\n");
        return false;
    }
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        return 0;
    }
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);
    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    // Create one OpenGL texture
    GLuint textureID = 0;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return textureID;
}
