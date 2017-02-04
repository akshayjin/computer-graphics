#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>
#include <string> 
#include "include/PLYModel.h"

#define PI 3.14159265
 
PLYModel::PLYModel()
{
 
}
 
 
float* PLYModel::calculateNormal( float *coord1, float *coord2, float *coord3 )
{
   /* calculate Vector1 and Vector2 */
   float va[3], vb[3], vr[3], val;
   va[0] = coord1[0] - coord2[0];
   va[1] = coord1[1] - coord2[1];
   va[2] = coord1[2] - coord2[2];
 
   vb[0] = coord1[0] - coord3[0];
   vb[1] = coord1[1] - coord3[1];
   vb[2] = coord1[2] - coord3[2];
 
   /* cross product */
   vr[0] = va[1] * vb[2] - vb[1] * va[2];
   vr[1] = vb[0] * va[2] - va[0] * vb[2];
   vr[2] = va[0] * vb[1] - vb[0] * va[1];
 
   /* normalization factor */
   val = sqrt( vr[0]*vr[0] + vr[1]*vr[1] + vr[2]*vr[2] );
 
	float *norm = new float[3];
	norm[0] = vr[0]/val;
	norm[1] = vr[1]/val;
	norm[2] = vr[2]/val;
 
 
	return norm;
}
 
 
 
int PLYModel::Load(char* filename)
{
    this->TotalConnectedTriangles = 0; 
	this->TotalConnectedPoints = 0;
 
    char* pch = strstr(filename,".ply");
    if (pch != NULL)
    {
	   FILE* file = fopen(filename,"r");
	   if (file == NULL){
			printf("File could not be opened. Check file path: %s",filename);
			return 0;
		}
		fseek(file,0,SEEK_END);
		long fileSize = ftell(file);
		try
		{
			Vertex_Buffer = (float*) malloc (ftell(file));
		}
		catch (char* )
		{
			return -1;
		}
		if (Vertex_Buffer == NULL) return -1;
		fseek(file,0,SEEK_SET); 
 
	   Faces_Triangles = (float*) malloc(fileSize*sizeof(float));
	   Normals  = (float*) malloc(fileSize*sizeof(float));
	   Faces_TextureUV = (float*) malloc(fileSize*sizeof(float));
		
       if (file)
       {
			int i = 0;   
			int temp = 0;
			int quads_index = 0;
            int triangle_index = 0;
			int normal_index = 0;
			int texture_index = 0;
			char buffer[1000];
 
			fgets(buffer,300,file);
 
			// Find number of vertexes
			while (  strncmp( "element vertex", buffer,strlen("element vertex")) != 0  )
			{
				fgets(buffer,300,file);
			}
			strcpy(buffer, buffer+strlen("element vertex"));
			sscanf(buffer,"%i", &this->TotalConnectedPoints);
 
 
			// Find number of faces
			fseek(file,0,SEEK_SET);
			while (  strncmp( "element face", buffer,strlen("element face")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}
			strcpy(buffer, buffer+strlen("element face"));
			sscanf(buffer,"%i", &this->TotalFaces);
 
 
			// go to end_header
			while (  strncmp( "end_header", buffer,strlen("end_header")) != 0  )
			{
				fgets(buffer,300,file);			// format
			}
 
 
			// read vertices
			i =0;
			x_min = 100000;
			x_max = -100000;
			y_min = 100000;
			y_max = -100000;
			z_min = 100000;
			z_max = -100000;
			for (int iterator = 0; iterator < this->TotalConnectedPoints; iterator++)
			{
				fgets(buffer,300,file);
 
				sscanf(buffer,"%f %f %f", &Vertex_Buffer[i], &Vertex_Buffer[i+1], &Vertex_Buffer[i+2]);
				
				// Find Range
				if(Vertex_Buffer[i]<x_min)
					x_min = Vertex_Buffer[i];
				if(Vertex_Buffer[i]>x_max)
					x_max = Vertex_Buffer[i];
				if(Vertex_Buffer[i+1]<y_min)
					y_min = Vertex_Buffer[i+1];
				if(Vertex_Buffer[i+1]>y_max)
					y_max = Vertex_Buffer[i+1];
				if(Vertex_Buffer[i+2]<z_min)
					z_min = Vertex_Buffer[i+2];
				if(Vertex_Buffer[i+2]>z_max)
					z_max = Vertex_Buffer[i+2];
				i += 3;
			}
			i=0;
			for (int iterator = 0; iterator < this->TotalConnectedPoints; iterator++)
			{
				Vertex_Buffer[i] = (Vertex_Buffer[i]-x_min)/(x_max-x_min) - centerX;
				Vertex_Buffer[i+1] = (Vertex_Buffer[i+1]-y_min)/(y_max-y_min) - centerY; 
				Vertex_Buffer[i+2] = (Vertex_Buffer[i+2]-z_min)/(z_max-z_min) - centerZ; 
				i += 3;
			}
			
			// read faces
			i =0;
			for (int iterator = 0; iterator < this->TotalFaces; iterator++)
			{
				fgets(buffer,300,file);
 
					if (buffer[0] == '3')
					{
 
						int vertex1 = 0, vertex2 = 0, vertex3 = 0;
						buffer[0] = ' ';
						sscanf(buffer,"%i%i%i", &vertex1,&vertex2,&vertex3 );
 
						Faces_Triangles[triangle_index] = Vertex_Buffer[3*vertex1];
						Faces_Triangles[triangle_index+1] = Vertex_Buffer[3*vertex1+1];
						Faces_Triangles[triangle_index+2] = Vertex_Buffer[3*vertex1+2];
						Faces_Triangles[triangle_index+3] = Vertex_Buffer[3*vertex2];
						Faces_Triangles[triangle_index+4] = Vertex_Buffer[3*vertex2+1];
						Faces_Triangles[triangle_index+5] = Vertex_Buffer[3*vertex2+2];
						Faces_Triangles[triangle_index+6] = Vertex_Buffer[3*vertex3];
						Faces_Triangles[triangle_index+7] = Vertex_Buffer[3*vertex3+1];
						Faces_Triangles[triangle_index+8] = Vertex_Buffer[3*vertex3+2];
 
						float coord1[3] = { Faces_Triangles[triangle_index], Faces_Triangles[triangle_index+1],Faces_Triangles[triangle_index+2]};
						float coord2[3] = {Faces_Triangles[triangle_index+3],Faces_Triangles[triangle_index+4],Faces_Triangles[triangle_index+5]};
						float coord3[3] = {Faces_Triangles[triangle_index+6],Faces_Triangles[triangle_index+7],Faces_Triangles[triangle_index+8]};
						float *norm = this->calculateNormal( coord1, coord2, coord3 );
 
						Normals[normal_index] = norm[0];
						Normals[normal_index+1] = norm[1];
						Normals[normal_index+2] = norm[2];
						Normals[normal_index+3] = norm[0];
						Normals[normal_index+4] = norm[1];
						Normals[normal_index+5] = norm[2];
						Normals[normal_index+6] = norm[0];
						Normals[normal_index+7] = norm[1];
						Normals[normal_index+8] = norm[2];
						
						if(texture_fun==0)
						{
							Faces_TextureUV[texture_index] = Faces_Triangles[triangle_index]+centerX;
							Faces_TextureUV[texture_index+1] =(atan2((Faces_Triangles[triangle_index+1]+centerY),(Faces_Triangles[triangle_index+2]+centerZ))+PI)/(2*PI);
							Faces_TextureUV[texture_index+2] = Faces_Triangles[triangle_index+3]+centerX;
							Faces_TextureUV[texture_index+3] =(atan2((Faces_Triangles[triangle_index+4]+centerY),(Faces_Triangles[triangle_index+5]+centerZ))+PI)/(2*PI);
							Faces_TextureUV[texture_index+4] = Faces_Triangles[triangle_index+6]+centerX;
							Faces_TextureUV[texture_index+5] =(atan2((Faces_Triangles[triangle_index+7]+centerY),(Faces_Triangles[triangle_index+8]+centerZ))+PI)/(2*PI);
						}
						else
						{
							Faces_TextureUV[texture_index] = 0.5 + atan2f(Faces_Triangles[triangle_index+2]+centerX,Faces_Triangles[triangle_index]+centerX)/(2*PI);
							Faces_TextureUV[texture_index+1] = 0.5 - asin(Faces_Triangles[triangle_index+1]+centerY);
							Faces_TextureUV[texture_index+2] = 0.5 + atan2f(Faces_Triangles[triangle_index+5]+centerX,Faces_Triangles[triangle_index+3]+centerX)/(2*PI);
							Faces_TextureUV[texture_index+3] = 0.5 - asin(Faces_Triangles[triangle_index+4]+centerY);
							Faces_TextureUV[texture_index+4] = 0.5 + atan2f(Faces_Triangles[triangle_index+8]+centerX,Faces_Triangles[triangle_index+6]+centerX)/(2*PI);
							Faces_TextureUV[texture_index+5] = 0.5 - asin(Faces_Triangles[triangle_index+7]+centerY);
						}
						texture_index += 6; 
						normal_index += 9;
						triangle_index += 9;
						TotalConnectedTriangles += 3;
					}
 
 
					i += 3;
			}
 
 
			fclose(file);
		}
 
      else { printf("File could not be opened\n"); }
    } else {
      printf("File does not have a .PLY extension.\n");    
    }   
	return 0;
}
 
void PLYModel::Draw(GLuint Texture)
{
	if(textureON)
	{
		//~ GLuint Texture = loadBMP_custom(texture);
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
		glActiveTexture(GL_TEXTURE0);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glEnable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
		glVertexPointer(3,GL_FLOAT,	0,Faces_Triangles);
		glTexCoordPointer(2,GL_FLOAT,0,Faces_TextureUV);
		glNormalPointer(GL_FLOAT, 0, Normals);
		glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
		glDisable(GL_TEXTURE_2D);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);
		glEnableClientState(GL_VERTEX_ARRAY);	
		glEnableClientState(GL_NORMAL_ARRAY);
		glVertexPointer(3,GL_FLOAT,	0,Faces_Triangles);	
		glNormalPointer(GL_FLOAT, 0, Normals);
		glDrawArrays(GL_TRIANGLES, 0, TotalConnectedTriangles);	
		glDisableClientState(GL_VERTEX_ARRAY);    
		glDisableClientState(GL_NORMAL_ARRAY);
	}
	if(bb)
	{
		glPushMatrix();
			drawBB();
		glPopMatrix();
	}
}

void PLYModel::drawBB()
{
	glColor3f(0,0,1);
	glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
	glBegin(GL_QUADS);
		glColor3f(0,1,0);
		glVertex3f(-centerX,0,-centerZ);
		glVertex3f(-centerX,1,-centerZ);
		glVertex3f(-centerX,1,centerZ);
		glVertex3f(-centerX,0,centerZ);
		
		glVertex3f(centerX,0,-centerZ);
		glVertex3f(centerX,1,-centerZ);
		glVertex3f(centerX,1,centerZ);
		glVertex3f(centerX,0,centerZ);
		
		glVertex3f(-centerX,0,-centerZ);
		glVertex3f(centerX,0,-centerZ);
		glVertex3f(centerX,0,centerZ);
		glVertex3f(-centerX,0,centerZ);
		
		glVertex3f(-centerX,1,-centerZ);
		glVertex3f(centerX,1,-centerZ);
		glVertex3f(centerX,1,centerZ);
		glVertex3f(-centerX,1,centerZ);
	glEnd();
	glColor3f(1,1,1);
}

void PLYModel::setCenter(float cx,float cy,float cz)
{
	centerX = cx;
	centerY = cy;
	centerZ = cz;
}
