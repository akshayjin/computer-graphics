#include "include/Particle.h"
#include "include/Constants.h"
#include "glm/ext.hpp"
#include <glm/gtc/random.hpp>
#include <algorithm>
#include <iostream>

#define FLOOR_DEPTH 0

Particle::Particle(float mass, float radius, float offset, glm::vec4 pos)
{
	float jitter = 0.1;
	
	m_mass = mass;
	m_radius = radius;
	m_offset = offset;
	m_pos = glm::vec4(pos);
	m_velocity = glm::vec4(glm::linearRand(-jitter, jitter),0.0f,glm::linearRand(-jitter, jitter), 0.0f);
	m_omega = glm::vec4(0.0f);
	m_acc = glm::vec4(GRAVITY + WIND);
	m_angular_acc = glm::vec4(0.0f);
	m_orientation = glm::orientation(glm::vec3(0,1,0),glm::vec3(0,1,0));
}

/* Update center of mass position
*/

void Particle::update(float deltaT)
{
	// update velocity
	m_velocity += deltaT * m_acc;
	m_velocity *= AIR_RESISTANCE;
	
	// Collision with floor
	if(m_pos.y <= (FLOOR_DEPTH + m_radius + m_offset) && m_velocity.y < 0)
	{
		m_velocity.y = -RESTITUTION_COEFF * m_velocity.y;
		m_velocity.x = GROUND_FRICTION * m_velocity.x;
		m_velocity.z = GROUND_FRICTION * m_velocity.z;
	}
	
	// update position
	m_pos += deltaT * m_velocity;
	
	// update angular velocity
	m_omega += deltaT * m_angular_acc;
	
	// update orientation
	glm::vec3 axis(glm::normalize(m_omega));
	float angle = glm::length(m_omega);
	if(angle>0)
		glm::rotate(m_orientation, angle, axis);
	
	m_acc = GRAVITY + WIND;
	m_angular_acc =  glm::vec4(0.0f);
}

/* Detect and process collision with another particle
*  Allow some overlapping and use different contact forces
*  change acceleration after calculating net forces
*/

void Particle::processContact(Particle* neighbour)
{
	glm::vec4 offsets[4] = {glm::vec4 (m_offset,0,0,0),
							glm::vec4 (-m_offset,0,0,0),
							glm::vec4 (0,0,m_offset,0),
							glm::vec4 (0,0,-m_offset,0)};
	
	m_acc = GRAVITY + WIND;
	m_angular_acc =  glm::vec4(0.0f);
	
	// for every pair of sphere
	for(int i=0;i<4;i++)
	{
		glm::vec4 x1(m_pos + m_orientation * offsets[i]);
		for(int j=0;j<4;j++)
		{
			glm::vec4 x2(neighbour->m_pos + neighbour->m_orientation * offsets[j]);
			float overlap = std::max(0.0f, m_radius + neighbour->m_radius - glm::distance(x1,x2));
			
			if(overlap > 0.0001)		// Collision Detected
			{
				glm::vec4 normal((x2-x1)/glm::distance(x1,x2));
				glm::vec4 V(m_velocity - neighbour->m_velocity);
				float Vn = glm::dot(V,normal);
				//~ std::cout << "Vn = " << Vn << std::endl;
				//~ std::cout << "p1 = " << this << ", p2 = " << neighbour << std::endl;
				glm::vec4 Vt(V - Vn*normal);
				
				float fn = -(VISCOUS_DAMPING_COEFF*Vn + ELASTIC_RESTORATION_COEFF*overlap);
				glm::vec4 Fn( fn* normal);
				
				glm::vec4 Ft(0);
				//~ std::cout << "Vt = " << glm::to_string(Vt) << std::endl;
				if(glm::length(Vt)>0)
					Ft =  -1 * std::min(FRICTION_COEFF*fn, VISCOUS_DAMPING_COEFF_TANGENT*glm::length(Vt))*glm::normalize(Vt);

				// update acceleration (Global forces + normal force + tangential friction)
				m_acc += Fn / m_mass + Ft / m_mass;
				//~ std::cout << "m_acc = " << glm::to_string(m_acc)  << std::endl;
				
				// update angualar acceleration
				if(glm::length(Ft) > 0)
					m_angular_acc +=  (Ft / m_mass / (m_radius-overlap)) * glm::normalize(glm::vec4(glm::cross(glm::vec3(Ft), glm::vec3(normal)),0.0));
			}
		}
	}
}

/* Calculate poistion of each of the 4 sphere (x+Ro)
*/
void Particle::render()
{
	glm::vec4 offsets[4] = {glm::vec4 (m_offset,0,0,0),
							glm::vec4 (-m_offset,0,0,0),
							glm::vec4 (0,0,m_offset,0),
							glm::vec4 (0,0,-m_offset,0)};
	
	glColor3f(0.761,0.698,0.502);
	for(int i=0;i<4;i++)
	{
		glPushMatrix();
			glm::vec4 new_pos(m_pos + m_orientation * offsets[i]);
			glTranslatef(new_pos.x,new_pos.y,new_pos.z);
			glutSolidSphere(m_radius,2,2);
		glPopMatrix();
	}
}
