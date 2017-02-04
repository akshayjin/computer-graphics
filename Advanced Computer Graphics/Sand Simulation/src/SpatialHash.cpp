#include "include/SpatialHash.h"
#include <algorithm>
#include <iostream>

using namespace std;

SpatialHash::SpatialHash(float voxel_wid, int grid_wid)
{
	VOXEL_WIDTH = voxel_wid;
	GRID_WIDTH = grid_wid;
	hash_map = vector<vector<vector<vector<Particle*> > > > (XLIMIT,vector<vector<vector<Particle*> > >(YLIMIT, vector<vector<Particle*> >(ZLIMIT, vector<Particle*>())));
}

HASH_ID SpatialHash::hash_func(glm::vec4 pos)
{
	int x = ((int)((pos.x + GRID_WIDTH*0.4) / VOXEL_WIDTH) % XLIMIT + XLIMIT) % XLIMIT;
	int y = ((int)((pos.y + 1) / VOXEL_WIDTH ) % YLIMIT + YLIMIT) % YLIMIT;
	int z = ((int)((pos.z + GRID_WIDTH*0.4) / VOXEL_WIDTH) % ZLIMIT + ZLIMIT) % ZLIMIT;
	return HASH_ID(x,y,z);
}

Particle* SpatialHash::get_particle(HASH_ID voxelID, int pid)
{
	return hash_map[voxelID.x][voxelID.y][voxelID.z][pid];
}

void SpatialHash::add_particle(HASH_ID voxelID, Particle* p)
{
	hash_map[voxelID.x][voxelID.y][voxelID.z].push_back(p);
}

void SpatialHash::remove_particle(HASH_ID voxelID, Particle* p)
{
	int old_size = hash_map[voxelID.x][voxelID.y][voxelID.z].size();
	hash_map[voxelID.x][voxelID.y][voxelID.z].erase(std::remove(hash_map[voxelID.x][voxelID.y][voxelID.z].begin(), hash_map[voxelID.x][voxelID.y][voxelID.z].end(), p), hash_map[voxelID.x][voxelID.y][voxelID.z].end());
	//~ if(hash_map[voxelID.x][voxelID.y][voxelID.z].size() - old_size != 1)
		//~ std::cout << "FAILURE" << std::endl;
	//~ else
		//~ std::cout << "SUCCESS" << std::endl;
}
