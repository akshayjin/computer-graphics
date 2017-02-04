#include "DataHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#pragma warning(disable : 4305)
#pragma warning(disable : 4136)

#define FILE_TYPE 0		// 0-> obj file with no textures, 1-> obj file with textures

std::vector<std::vector<float>>  rabbit_vertices;
std::vector<std::vector<float>>  rabbit_texturemap;
std::vector<std::vector<int>>   rabbit_triangles;
std::vector<std::vector<int>>   rabbit_face_textures;

void LoadFromFile(FILE* file) 
{
	char ch;
	char buf[1024];
	int local_num_vertices = 0;
	int local_num_faces = 0;
	int local_num_texture_coords = 0;
	double x, y, z;        /* ax + by + cz = 0 */
	int a,b,c;
	int t1,t2,t3;
	int n1,n2,n3;

	while ( fscanf(file, "%c", &ch) != EOF )
	{
		switch(ch)
		{
		case ' ' :         /* blanks */
		case '\t':
		case '\r':
		case '\n':
			continue;
		case '#':          /* comment */
			fgets(buf, sizeof(buf), file);
			break;
		case 'v':          /* vertex */
		case 'V':
		{
									/* vertex index starts from 1 */
			fscanf(file, "%lf %lf %lf",	&x, &y, &z);					
			std::vector<float> vertex; vertex.push_back(x); vertex.push_back(y); vertex.push_back(z);
			rabbit_vertices.push_back(vertex);
			local_num_vertices++;	
			break;
		}
		case 't':          /* texture UV */
		case 'T':
		{
									/* vertex index starts from 1 */
			fscanf(file, "%lf %lf %lf",	&x, &y, &z);					
			std::vector<float> uv; uv.push_back(x); uv.push_back(y);// vertex.push_back(z);
			rabbit_texturemap.push_back(uv);
			local_num_texture_coords++;	
			break;
		}
		case 'f':          /* face */
		case 'F':
		{
			if(FILE_TYPE==0){
				fscanf(file, "%d %d %d",&a, &b, &c);
				std::vector<int> face; face.push_back(a-1); face.push_back(b-1); face.push_back(c-1);
				rabbit_triangles.push_back(face);
				local_num_faces++;	
			}
			else if(FILE_TYPE==1){
				fscanf(file, "%d/%d %d/%d %d/%d",&a,&t1, &b,&t2, &c,&t3);
				std::vector<int> face; face.push_back(a-1); face.push_back(b-1); face.push_back(c-1);
				rabbit_triangles.push_back(face);
				local_num_faces++;	
				
				std::vector<int> face_texture; face_texture.push_back(t1-1); face_texture.push_back(t2-1); face_texture.push_back(t3-1);
				rabbit_face_textures.push_back(face_texture);
			}
			break;
		}
		default:          /* invalid commands */
			std::cout << (int)ch  << std::endl;
			fgets(buf, sizeof(buf), file);
			fprintf(stderr, "Parse() failed: invalid attributes: \"%c\".\n", ch);
			exit(-2);
		}
	}
	std::cout << rabbit_vertices.size() <<" ,"<< rabbit_triangles.size()  << std::endl;
}
