#include <vector>
#include <stdio.h>  
 
extern std::vector<std::vector<float>>  rabbit_vertices;
extern std::vector<std::vector<int>>   rabbit_triangles;
extern std::vector<std::vector<float>>  rabbit_texturemap;
extern std::vector<std::vector<int>>   rabbit_face_textures;
extern void LoadFromFile(FILE* file);
