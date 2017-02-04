#pragma once

#define GLM_FORCE_RADIANS
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <GL/glut.h>	

#include <vector>

/*
 * A single sand particle looks like a rigid combination
 * of four symmetric spheres.
 */
 
class Particle
{
public:
	Particle(float mass, float radius, float offset, glm::vec4 pos);
	
	float m_mass;	
	float m_radius;
	float m_offset;
	glm::vec4 m_pos;
	glm::vec4 m_velocity;
	glm::vec4 m_omega;
	glm::vec4 m_acc;
	glm::vec4 m_angular_acc;
	glm::mat4x4 m_orientation;
	
	void update(float deltaT);
	void processContact(Particle* neighbour);
	void render();
};
