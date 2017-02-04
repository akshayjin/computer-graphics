#include "model.h"
#include <QtOpenGL>
#include <iostream>
#include <GL/gl.h>
#include "model.h"
#include <iostream>

using namespace std;

/*
 * Load all textures once at the beginning to save computation
 *
 * */
void Model::init()
{
    texture_files.push_back("Textures/checker.bmp");
    texture_files.push_back("Textures/grass.bmp");
    texture_files.push_back("Textures/skin.bmp");
    texture_files.push_back("Textures/stones.bmp");
    texture_files.push_back("Textures/wood.bmp");
    texture_files.push_back("Textures/greenapple.bmp");
    texture_files.push_back("Textures/dragon.bmp");
    for(int i=0;i<texture_files.size();i++)
    {
     texture_ids.push_back(loadBMP_custom(texture_files[i].c_str()));
    }
}

void Model::render()
{
    // Plot PLY file
    glPushMatrix();
        glTranslatef(-1.5,0,1.5);
        // Check if texture is enabled or not
        if(model3D_1.textureON)
        {
            model3D_1.Draw(texture_ids[model3D_1_texture]);
        }
        else
            model3D_1.Draw(0);
    glPopMatrix();

    glPushMatrix();
        glTranslatef(1.5,0,1.5);
        // Check if texture is enabled or not
        if(model3D_2.textureON)
        {
            model3D_2.Draw(texture_ids[model3D_2_texture]);
        }
        else
            model3D_2.Draw(0);
    glPopMatrix();

    glPushMatrix();
        drawFloor();
    glPopMatrix();

    glPushMatrix();
        glDisable(GL_LIGHTING);
        drawAxis();
    glPopMatrix();
}

/*
 * Draws a Rectangular Plain in XZ plain centered around origin
 * */
void Model::drawFloor()
{
    glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_ids[4]);
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
        glTexCoord2f(0,0);glVertex3f(-3,0,-3);
        glTexCoord2f(0,1);glVertex3f(-3,0,3);
        glTexCoord2f(1,1);glVertex3f(3,0,3);
        glTexCoord2f(1,0);glVertex3f(3,0,-3);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

/*
 * Draws XYZ axis with RGB colors respectively
 * */
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
}

void Model::translate(float x, float y)
{
}

/*
 * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
 *
 * */
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
