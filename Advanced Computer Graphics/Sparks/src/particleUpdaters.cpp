#include "include/particleUpdaters.h"
#include <assert.h>
#include <algorithm>
#include <iostream>
#include <glm/common.hpp>
#include <glm/gtc/random.hpp>
#include <stdlib.h>
#include <time.h>  

namespace particles
{
	namespace updaters
	{
		bool isCollindingCone(glm::vec4 point);
		int isCollindingBunny(glm::vec4 p);
		
        void EulerUpdater::update(double dt, ParticleData *p)
		{
			const glm::vec4 globalA{ dt * m_globalAcceleration.x, dt * m_globalAcceleration.y, dt * m_globalAcceleration.z, 0.0 };
			const float localDT = (float)dt;

            glm::vec4 *  acc = p->m_acc.get();
            glm::vec4 *  vel = p->m_vel.get();
            glm::vec4 *  pos = p->m_pos.get();

			const unsigned int endId = p->m_countAlive;
			for (size_t i = 0; i < endId; ++i)
				acc[i] += globalA;

			for (size_t i = 0; i < endId; ++i)
				vel[i] += localDT * acc[i];

			for (size_t i = 0; i < endId; ++i)
				pos[i] += localDT * vel[i];
		}

        void FloorUpdater::update(double dt, ParticleData *p)
		{
			const float localDT = (float)dt;

            glm::vec4 *  acc = p->m_acc.get();
            glm::vec4 *  vel = p->m_vel.get();
            glm::vec4 *  pos = p->m_pos.get();

			const size_t endId = p->m_countAlive;
			srand (time(NULL));
			for (size_t i = 0; i < endId; ++i)
			{
				if (pos[i].y < m_floorY)
				{
					glm::vec4 force = p->m_acc[i];
					float normalFactor = glm::dot(force, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
					if (normalFactor < 0.0f)
						force -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * normalFactor;

					float velFactor = glm::dot(vel[i], glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
					//if (velFactor < 0.0)
					// Radomize bounce factor
					vel[i] -= glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * (1.0f + m_bounceFactor*glm::linearRand(0.8f, 1.0f)) * velFactor;
					
					// Add random jitter to velocity
					vel[i] += glm::vec4(glm::ballRand(1.0f), 1) * glm::vec4(0.2f, 0.0f, 0.2f, 0.0f);
					
					acc[i] = force; 
				}
			}

		}
		
		void ObjectCollisionUpdater::update(double dt, ParticleData *p)
		{
			const float localDT = (float)dt;

            glm::vec4 *  acc = p->m_acc.get();
            glm::vec4 *  vel = p->m_vel.get();
            glm::vec4 *  pos = p->m_pos.get();

			const size_t endId = p->m_countAlive;
			srand (time(NULL));
			for (size_t i = 0; i < endId; ++i)
			{
				// Collision with cube
				if (pos[i].x > -0.42 && pos[i].x < 0.42 && pos[i].y > -2.1 && pos[i].y < -1.28 && pos[i].z > 3.18 && pos[i].z < 4.02)
				{
					float m_bounceFactor = 0.6;
					// Colliding with up face
					if(pos[i].y > -1.35){
						glm::vec4 normal = glm::vec4(0, 1, 0, 0);
						vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
						vel[i].y *= m_bounceFactor;
					}
					
					// Colliding with front face
					else if(pos[i].z < 3.25){
						glm::vec4 normal(0, 0, -1, 0);
						vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
						vel[i].z *= m_bounceFactor;
					}
					
					// Add random jitter to velocity (friction)
					vel[i] += glm::vec4(glm::ballRand(1.0f), 1) * glm::vec4(0.5f, 0.0f, -0.1f, 0.0f);
				}
				// Collision with sphere
				else if (glm::distance(pos[i],glm::vec4(1, -1.45, 2.6, pos[i].w)) < 0.65)
				{
					glm::vec4 normal = pos[i] - glm::vec4(1, -1.45, 2.6, pos[i].w);
					normal = glm::normalize(normal);
					vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
					vel[i] += glm::vec4(glm::linearRand(0.0f, 0.5f)-0.5f,glm::linearRand(0.0f, 0.5f)-0.5f,0,0);
				}
				// Collision with cone
				else if (isCollindingCone(pos[i]))
				{
					float m_bounceFactor = 0.6;
					if(pos[i].z < 4.1)
						vel[i] *= glm::vec4(1,1,-1,1)*m_bounceFactor;
					else
						vel[i] *= m_bounceFactor;
					vel[i] += glm::vec4(glm::linearRand(0.5f, 1.0f),glm::linearRand(0.5f, 1.0f),0,0);
				}
				
				// Collision with bunny (approximate by bounding boxes)
				else if (isCollindingBunny(pos[i])>0)
				{
					float m_bounceFactor = 0.6;
					
					switch (isCollindingBunny(pos[i])) 
					{
					case 1:					// body
						// front face
						if(pos[i].z < 3.2){
							glm::vec4 normal(0, 0, -1, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// up face
						else if(pos[i].y > -1.0) {
							glm::vec4 normal(0, 1, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// right face
						else if(pos[i].x > -0.5) {
							glm::vec4 normal(-1, 0, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						
						break;
					case 2:					// head
						// front face
						if(pos[i].z < 4.4){
							glm::vec4 normal(0, 0, -1, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// right face
						else if(pos[i].x > -0.3) {
							glm::vec4 normal(-1, 0, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// up face
						else if(pos[i].y > -0.46) {
							glm::vec4 normal(0, 1, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						break;
					case 3:				// ear far
						// front face
						if(pos[i].z < 4.85){
							glm::vec4 normal(0, 0, -1, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// right face
						else if(pos[i].x > -1.1) {
							glm::vec4 normal(-1, 0, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// up face
						else if(pos[i].y > -0.2) {
							glm::vec4 normal(0, 1, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						break;
					case 4:				// ear near
						// front face
						if(pos[i].z < 4.55){
							glm::vec4 normal(0, 0, -1, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// right face
						else if(pos[i].x > -1.2) {
							glm::vec4 normal(-1, 0, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						// up face
						else if(pos[i].y > -0.1) {
							glm::vec4 normal(0, 1, 0, 0);
							vel[i] = vel[i] - 2*(glm::dot(vel[i],normal))*normal;
							vel[i] *= m_bounceFactor;
						}
						break;
					}
					vel[i] += glm::vec4(glm::ballRand(1.0f), vel[i].w);
				}
				
			}

		}
		
		bool isCollindingCone(glm::vec4 point){
			if(point.y>-1.1 || point.z<3.1)
				return false;
			glm::vec4 tip(1.5, -1.1, 4.1, point.w);
			float cone_dist = glm::dot(point - tip, glm::vec4(0,-1,0,0));
			float cone_radius = (cone_dist) * 0.99;
			float orth_distance = glm::length((point - tip) - cone_dist * glm::vec4(0,-1,0,0));
			//~ std::cout << "" << std::endl;
			return orth_distance < cone_radius;
		}
		
		int isCollindingBunny(glm::vec4 p){
			//body
			if(p.x > -1.55 && p.x < -0.4 && p.y > -2.1 && p.y < -0.8 && p.z > 3.18 && p.z < 4.6)
				return 1;
			
			//head
			if(p.x > -1.0 && p.x < -0.2 && p.y > -0.8 && p.y < -0.45 && p.z > 4.3 && p.z < 4.9)
				return 2;
				
			//ear far
			if(p.x > -1.6 && p.x < -1.0 && p.y > -0.6 && p.y < -0.1 && p.z > 4.75 && p.z < 5.1)
				return 3;
				
			//ear near
			if(p.x > -1.65 && p.x < -1.1 && p.y > -0.6 && p.y < 0.0 && p.z > 4.15 && p.z < 4.45)
				return 4;
			
			return 0;
		}

        void BasicColorUpdater::update(double dt, ParticleData *p)
		{
            glm::vec4 *  col = p->m_col.get();
            glm::vec4 *  startCol = p->m_startCol.get();
            glm::vec4 *  endCol = p->m_endCol.get();
            glm::vec4 *  t = p->m_time.get();

			const size_t endId = p->m_countAlive;
			for (size_t i = 0; i < endId; ++i)
				col[i] = glm::mix(startCol[i], endCol[i], t[i].z);
		}

        void PosColorUpdater::update(double dt, ParticleData *p)
		{
            glm::vec4 *  col = p->m_col.get();
            glm::vec4 *  startCol = p->m_startCol.get();
            glm::vec4 *  endCol = p->m_endCol.get();
            glm::vec4 *  t = p->m_time.get();
            glm::vec4 *  pos = p->m_pos.get();

			const int endId = (int)p->m_countAlive;
            float scaler, scaleg, scaleb;
			float diffr = m_maxPos.x - m_minPos.x;
			float diffg = m_maxPos.y - m_minPos.y;
			float diffb = m_maxPos.z - m_minPos.z;

            for (int i = 0; i < endId; ++i)
            {
                scaler = (pos[i].x - m_minPos.x) / diffr;
                scaleg = (pos[i].y - m_minPos.y) / diffg;
                scaleb = (pos[i].z - m_minPos.z) / diffb;
                col[i].r = scaler;// glm::mix(p->m_startCol[i].r, p->m_endCol[i].r, scaler);
                col[i].g = scaleg;// glm::mix(p->m_startCol[i].g, p->m_endCol[i].g, scaleg);
                col[i].b = scaleb;// glm::mix(p->m_startCol[i].b, p->m_endCol[i].b, scaleb);
                col[i].a = glm::mix(startCol[i].a, endCol[i].a, t[i].z);
            }
		}

        void VelColorUpdater::update(double dt, ParticleData *p)
		{
            glm::vec4 *  col = p->m_col.get();
            glm::vec4 *  startCol = p->m_startCol.get();
            glm::vec4 *  endCol = p->m_endCol.get();
            glm::vec4 *  t = p->m_time.get();
            glm::vec4 *  vel = p->m_vel.get();

			const size_t endId = p->m_countAlive;
			float scaler, scaleg, scaleb;
			float diffr = m_maxVel.x - m_minVel.x;
			float diffg = m_maxVel.y - m_minVel.y;
			float diffb = m_maxVel.z - m_minVel.z;
			for (size_t i = 0; i < endId; ++i)
			{
				scaler = (vel[i].x - m_minVel.x) / diffr;
				scaleg = (vel[i].y - m_minVel.y) / diffg;
				scaleb = (vel[i].z - m_minVel.z) / diffb;
				col[i].r = scaler;
				col[i].g = scaleg;
				col[i].b = scaleb;
				col[i].a = glm::mix(startCol[i].a, endCol[i].a, t[i].z);
			}
		}

        void BasicTimeUpdater::update(double dt, ParticleData *p)
		{
			unsigned int endId = p->m_countAlive;
			const float localDT = (float)dt;

            glm::vec4 *  t = p->m_time.get();

			if (endId == 0) return;

			for (size_t i = 0; i < endId; ++i)
			{
				t[i].x -= localDT;
				// interpolation: from 0 (start of life) till 1 (end of life)
				t[i].z = (float)1.0 - (t[i].x*t[i].w); // .w is 1.0/max life time		

				if (t[i].x < (float)0.0)
				{
					p->kill(i);
					endId = p->m_countAlive < p->m_count ? p->m_countAlive : p->m_count;
				}
			}
		}
	}
}
