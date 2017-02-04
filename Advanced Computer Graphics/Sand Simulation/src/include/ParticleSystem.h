#pragma once
#include "Particle.h"
#include "SpatialHash.h"
#include <glm/vec4.hpp>

/*
 * A single sand particle looks like a rigid combination
 * of four symmetric spheres.
 */
 
class ParticleSystem
{
public:
	ParticleSystem(int numParticles);
	~ParticleSystem();
	
	int NUM_PARTICLES;
	std::vector<Particle*> particles;
	std::vector<glm::vec4> particles_pos;
	SpatialHash hash;
	float render_buffer[50000][60][9];
	float color_buffer[50000][60][12];
	
	void generate();
	void update();
	void render();
};

