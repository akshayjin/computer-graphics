#pragma once
#include <vector>
#include "Particle.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#define XLIMIT 20
#define YLIMIT 500
#define ZLIMIT 20

typedef struct hash_id{
	int x;
	int y;
	int z;
	hash_id(int a, int b, int c): x(a), y(b), z(c){}
}HASH_ID;

/*
 * made of voxels of size two times the maximum particle radius
 */
 
class SpatialHash
{
public:
	SpatialHash(float voxel_wid, int grid_wid);
	
	float VOXEL_WIDTH;
	int GRID_WIDTH;
	std::vector< std::vector <std::vector <std::vector <Particle*> > > > hash_map;
	
	HASH_ID hash_func(glm::vec4 pos);
	Particle* get_particle(HASH_ID voxelID, int pid);
	void add_particle(HASH_ID voxelID, Particle* p);
	void remove_particle(HASH_ID voxelID, Particle* p);
};


