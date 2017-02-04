#include "include/ParticleSystem.h"
#include <math.h>
#include <iostream>
#include "include/Sand_Particle.h"
#include "glm/ext.hpp"
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#define GRID_WIDTH 6
#define COLLISION_ON 1	// 1->ON 0->OFF
#define BALL_ON 1	// 1->ON 0->OFF

ParticleSystem::ParticleSystem(int numParticles): NUM_PARTICLES(numParticles), hash(0.4, GRID_WIDTH)
{
	generate();
}

ParticleSystem::~ParticleSystem()
{
	for (int i =0; i< particles.size();i++)
	{
		delete (particles[i]);
	} 
   particles.clear();
}


void ParticleSystem::generate()
{
	int numLayers = NUM_PARTICLES/GRID_WIDTH/GRID_WIDTH;
	int startX = -GRID_WIDTH/2*0.4;
	int startY = 2;
	int startZ = -GRID_WIDTH/2*0.4;
	
	int particles_generated = 0;
	for(int j=0; j<numLayers; j++)
	{
		for(int i=0;i<GRID_WIDTH*GRID_WIDTH;i++)
		{
			if(particles_generated == NUM_PARTICLES)
				break;
			// Generate and add a particle
			Particle* p = new Particle(1,.1,.1,glm::vec4(startX + .4*(i%GRID_WIDTH),startY + j*0.2, startZ + .4*((int)i/GRID_WIDTH),1));
			particles.push_back(p);
			particles_pos.push_back(p->m_pos);
			particles_generated++;
			
			// Add to spatial hash
			HASH_ID hashID = hash.hash_func(p->m_pos);
			//~ std::cout << "hash_id = " << hashID.x << ", " << hashID.y << ", " << hashID.z << std::endl;
			hash.add_particle(hashID,p);
		}
	}
	
	if(BALL_ON)
	{
		// Add a BIG particle
		Particle* p = new Particle(8,0.2,0.2,glm::vec4(startX + GRID_WIDTH/4 + 0.4, startY + 5 + numLayers*0.2,startZ + GRID_WIDTH/4 + 0.4, 1));
		particles.push_back(p);
		particles_pos.push_back(p->m_pos);
		hash.add_particle(hash.hash_func(p->m_pos),p);
	}
	
	// Add color
	srand (time(NULL));
	for (int i =0; i< particles.size();i++)
	{
		for(int j =0; j< 60; j++)
		{
			for(int k =0;k<3;k++)
			{
				double r = (((double) rand() / (RAND_MAX)) - 0.5)/2;
				color_buffer[i][j][4*k+0] = 0.761 + r;
				color_buffer[i][j][4*k+1] = 0.698 + r;
				color_buffer[i][j][4*k+2] = 0.502 + r;
				color_buffer[i][j][4*k+3] = 1.0;
			}
			if(BALL_ON && i==particles.size()-1)
			{
				for(int k =0;k<3;k++)
				{
					color_buffer[i][j][4*k+0] = 1.0;
					color_buffer[i][j][4*k+1] = 0.0;
					color_buffer[i][j][4*k+2] = 0.0;
					color_buffer[i][j][4*k+3] = 1.0;
				}
			}
		}
	}
	
}


void ParticleSystem::update()
{
	if(COLLISION_ON)
	{
		// Check surrounding voxels and process contact
		for (int i =0; i< particles.size();i++)
		{
			int numIntersections = -1;
			HASH_ID hashID = hash.hash_func(particles[i]->m_pos);
			for(int x = hashID.x-1; x<=hashID.x+1; x++)
			{
				for(int y = hashID.y-1; y<=hashID.y+1; y++)
				{
					for(int z = hashID.z-1; z<=hashID.z+1; z++)
					{
						int a = (x % XLIMIT + XLIMIT ) % XLIMIT;
						int b = (y % YLIMIT + YLIMIT ) % YLIMIT;
						int c = (z % ZLIMIT + ZLIMIT ) % ZLIMIT;
						for(int j=0;j<hash.hash_map[a][b][c].size();j++)
						{
							if(particles[i] != hash.get_particle(HASH_ID(a,b,c), j))
								particles[i]->processContact( hash.get_particle(HASH_ID(a,b,c),j));
							numIntersections++;
						}
					}
				}
			}
		}
	}
	
	// Update position and spatial hash ID
	for (int i =0; i< particles.size();i++)
	{
		HASH_ID oldHashID = hash.hash_func(particles[i]->m_pos);
		particles[i]->update(1.0/60.0);
		particles_pos[i] = particles[i]->m_pos;
		HASH_ID newHashID = hash.hash_func(particles[i]->m_pos);
		if(newHashID.x<0 || newHashID.y<0 || newHashID.z<0 || newHashID.x>=50 || newHashID.y>=100 || newHashID.z>=50)
			continue;
		if(oldHashID.x<0 || oldHashID.y<0 || oldHashID.z<0 || oldHashID.x>=50 || oldHashID.y>=100 || oldHashID.z>=50)
			continue;
		hash.add_particle(newHashID,particles[i]);
		hash.remove_particle(oldHashID,particles[i]);
	} 
	
	// Update render buffer
	int scale  = 40;
	for (int i =0; i< particles.size();i++)
	{
		if(BALL_ON && i==particles.size()-1)
			scale = 8;
		for(int j =0; j< 60; j++)
		{
			render_buffer[i][j][0] = face_vertices[j][0]/scale + particles[i]->m_pos.x;
			render_buffer[i][j][1] = face_vertices[j][1]/scale + particles[i]->m_pos.y;
			render_buffer[i][j][2] = face_vertices[j][2]/scale + particles[i]->m_pos.z;
			
			render_buffer[i][j][3] = face_vertices[j][3]/scale + particles[i]->m_pos.x;
			render_buffer[i][j][4] = face_vertices[j][4]/scale + particles[i]->m_pos.y;
			render_buffer[i][j][5] = face_vertices[j][5]/scale + particles[i]->m_pos.z;
			
			render_buffer[i][j][6] = face_vertices[j][6]/scale + particles[i]->m_pos.x;
			render_buffer[i][j][7] = face_vertices[j][7]/scale + particles[i]->m_pos.y;
			render_buffer[i][j][8] = face_vertices[j][8]/scale + particles[i]->m_pos.z;
			
			
		}
	}
}

void ParticleSystem::render()
{
	//~ glColor3f(0.761,0.698,0.502);
	//~ glEnableClientState(GL_VERTEX_ARRAY);	
		//~ glVertexPointer(4,GL_FLOAT, 0, particles_pos.data());	
		//~ glPointSize(3);
		//~ glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);	
	//~ glDisableClientState(GL_VERTEX_ARRAY);    
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState( GL_COLOR_ARRAY );
		glVertexPointer(3, GL_FLOAT, 0, render_buffer);
		glColorPointer(4,GL_FLOAT,0, color_buffer);	
		glDrawArrays(GL_TRIANGLES, 0, particles.size()*60*3);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState( GL_COLOR_ARRAY );

}
