#include <stdio.h>  
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <algorithm> 
#include <ctime>        
#include <cstdlib> 
#include <vector>
#include "vector.h"
#include "progmesh.h"
#include "DataHandler.h"
#include "SOIL.h"
#include <GL/gl.h>		
#include <GL/glu.h>		
#include <GL/glut.h>

int   render_num;   
List<Vector> vert; 
List<tridata> tri; 
List<int> collapse_map; 
List<int> triangle_chart_map; 
int renderpolycount=0; 
int boundary_len = 0;
Vector model_position;         
Quaternion model_orientation;
float scalex=1.0f;
float scaley=1.0f;
float scalez=1.0f;
bool drawWireframe = true;

class Color {
  public:
	float r;
	float g;
	float b;
	Color(float x,float y, float z);
};
Color::Color(float x,float y, float z){
	r=x;g=y;b=z;
}

std::vector<Color> pallete;

void fill_pallete()
{
	for(int r=0;r<25;r++){
		for(int g=0;g<25;g++){
			for(int b=0;b<25;b++){
				Color c(r/25.0,g/25.0,b/25.0);
				pallete.push_back(c);
			}
		}
	}
	std::srand ( unsigned ( std::time(0) ) );
	std::random_shuffle ( pallete.begin(), pallete.end() );
}

int Map(int a,int mx) {
	if(mx<=0) return 0;
	while(a>=mx) {  
		a=collapse_map[a];
	}
	return a;
}

void DrawModelTriangles() {
	assert(collapse_map.num);
	renderpolycount=0;
	int i=0;
	glEnable(GL_TEXTURE_2D);
	for(i=0;i<tri.num;i++) {
		int p0= Map(tri[i].v[0],render_num);
		int p1= Map(tri[i].v[1],render_num);
		int p2= Map(tri[i].v[2],render_num);
		int t1 = tri[i].t[0];
		int t2 = tri[i].t[1];
		int t3 = tri[i].t[2];
		
		if (p0==p1 || p1==p2 || p2==p0) continue;
		renderpolycount++;
		Vector v0,v1,v2; 
		v0 = vert[p0];
		v1 = vert[p1];
		v2 = vert[p2];
		glBegin(GL_POLYGON);
			Vector nrml = (v1-v0) * (v2-v1);  // cross product
			if(0<magnitude(nrml)) {
				glNormal3fv(normalize(nrml));
			}
			//~ glColor3f(1,1,0);
			glColor3f(pallete[triangle_chart_map[i]].r,pallete[triangle_chart_map[i]].g,pallete[triangle_chart_map[i]].b);
			//~ glTexCoord2d(rabbit_texturemap[t1][0],rabbit_texturemap[t1][1]);
			glVertex3fv(v0);
			//~ glTexCoord2d(rabbit_texturemap[t2][0],rabbit_texturemap[t2][1]);
			glVertex3fv(v1);
			//~ glTexCoord2d(rabbit_texturemap[t2][0],rabbit_texturemap[t2][1]);
			glVertex3fv(v2);
		glEnd();
		if(drawWireframe)
		{
			glBegin(GL_LINES);
				glColor3f(1,0,0);
				glVertex3fv(v0);
				glVertex3fv(v1);
				glVertex3fv(v1);
				glVertex3fv(v2);
				glVertex3fv(v0);
				glColor3f(1,1,1);
			glEnd();
		}
	}
	//~ glDisable(GL_TEXTURE_2D);
}


void PermuteVertices(List<int> &permutation) {
	// rearrange the vertex list 
	List<Vector> temp_list;
	int i;
	assert(permutation.num==vert.num);
	for(i=0;i<vert.num;i++) {
		temp_list.Add(vert[i]);
	}
	for(i=0;i<vert.num;i++) {
		vert[permutation[i]]=temp_list[i];
	}
	// update the changes in the entries in the triangle list
	for(i=0;i<tri.num;i++) {
		for(int j=0;j<3;j++) {
			tri[i].v[j] = permutation[tri[i].v[j]];
		}
	}
}

void GetModelData(){
	int i;
	FILE *file;
	if ((file = fopen("bunny_low.obj", "r")) == NULL)
	{
		fprintf(stderr, "write_pm() failed: can't open  data file model.obj.\n");
		exit(-1);
	}
	LoadFromFile(file);
	std::cout << rabbit_vertices.size() <<" vertices, "<< rabbit_triangles.size()  <<  " faces" << std::endl;
	
	fclose(file);
	
	for(i=0;i<rabbit_vertices.size();i++) {
		std::vector<float> vp=rabbit_vertices[i];
		vert.Add(Vector(vp[0],vp[1],vp[2]));
	}
	
	for(i=0;i<rabbit_triangles.size();i++) {
		tridata td;
		td.v[0]=rabbit_triangles[i][0];
		td.v[1]=rabbit_triangles[i][1];
		td.v[2]=rabbit_triangles[i][2];
		//~ td.t[0]=rabbit_face_textures[i][0];
		//~ td.t[1]=rabbit_face_textures[i][1];
		//~ td.t[2]=rabbit_face_textures[i][2];
		tri.Add(td);
	}
	render_num=vert.num;  // by default lets use all the model to render
	
	//~ GLuint tex_2d = SOIL_load_OGL_texture (
		//~ "EAGLE2.jpg",
		//~ SOIL_LOAD_AUTO,
		//~ SOIL_CREATE_NEW_ID,
		//~ SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	//~ );
	//~ if (tex_2d == 0)
	//~ {
		//~ fprintf(stderr, "load texture() failed\n");
		//~ exit(-1);
	//~ }
	//~ glBindTexture(GL_TEXTURE_2D, tex_2d);
}

void InitModel() {
	List<int> permutation;
	GetModelData();  
	fill_pallete();
	ProgressiveMesh(vert,tri,collapse_map,permutation, triangle_chart_map, boundary_len);
	PermuteVertices(permutation);
	model_position    = Vector(0,-0.25,-1);
	Quaternion yaw(Vector(0,1,0),-3.14f/4);    // 45 degrees
	Quaternion pitch(Vector(1,0,0),3.14f/12);  // 15 degrees 
	model_orientation = pitch*yaw;
}

void RenderModel() {
	glEnable(GL_DEPTH_TEST);
	glColor3f(1,1,1);
	glPushMatrix();
		glTranslatef(model_position.x,model_position.y,model_position.z);
		glScalef(scalex,scaley,scalez);
		Vector axis=model_orientation.axis();
		float angle=model_orientation.angle()*180.0f/3.14f;
		glRotatef(angle,axis.x,axis.y,axis.z);
		DrawModelTriangles();
	glPopMatrix();
	
	glPushMatrix();
		char numFaces[32] = "# Faces: ";
		char integer_string[16];
		sprintf(integer_string, "%d", renderpolycount);
		strcat(numFaces, integer_string);
		glRasterPos3f( 1, 1, -3); // location to start printing text
		for( int ch=0; ch < strlen(numFaces); ch++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, numFaces[ch] );
	glPopMatrix();
	
	glPushMatrix();
		char numCharts[32] = "# Charts: ";
		char numCharts_string[16];
		sprintf(numCharts_string, "%d", NUM_CHARTS);
		strcat(numCharts, numCharts_string);
		glRasterPos3f( 1, 0.85, -3); // location to start printing text
		for( int ch=0; ch < strlen(numCharts); ch++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, numCharts[ch] );
	glPopMatrix();
}

/***********Keyboard Interrupt*****************/

void KeyDown(unsigned char key, int x, int y)
{
	switch (key) 
	{
	case 27:		//ESC
		exit(0);
		break;
	case 'a':		
		render_num+=50;
		if(render_num>vert.num) render_num=vert.num;
		glutPostRedisplay();
		break;
	case 's':		
		render_num-=50;
		if(render_num<boundary_len*0) render_num=boundary_len*0;
		glutPostRedisplay();
		break;
	case 'w':		
		drawWireframe = !drawWireframe;
		glutPostRedisplay();
		break;
	case '>':
		scalex *= 1.25;
		scaley *= 1.25;
		scalez *= 1.25;
		glutPostRedisplay();
		break;
	case '<':
		scalex *= 0.8;
		scaley *= 0.8;
		scalez *= 0.8;
		glutPostRedisplay();
		break;
	}
}
