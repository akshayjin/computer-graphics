#pragma once

#include <vector>
#include "particles.h"

namespace particles
{
	namespace updaters
	{
		class EulerUpdater : public particles::ParticleUpdater
		{
		public:
			glm::vec4 m_globalAcceleration;
		public:
			EulerUpdater() : m_globalAcceleration(0.0) { }

			virtual void update(double dt, ParticleData *p) override;
		};

		// collision with the floor
		class FloorUpdater : public particles::ParticleUpdater
		{
		public:
			float m_floorY;
			float m_bounceFactor;
		public:
			FloorUpdater() : m_floorY(-2.05), m_bounceFactor(0.7f) { }

			virtual void update(double dt, ParticleData *p) override;
		};
		
		// collision with the scene objects
		class ObjectCollisionUpdater : public particles::ParticleUpdater
		{
		public:
			virtual void update(double dt, ParticleData *p) override;
		};

		class BasicColorUpdater : public ParticleUpdater
		{
		public:
			virtual void update(double dt, ParticleData *p) override;
		};

		class PosColorUpdater : public ParticleUpdater
		{
		public:
			glm::vec4 m_minPos;
			glm::vec4 m_maxPos;
		public:
			PosColorUpdater() : m_minPos(0.0), m_maxPos(1.0) { }

			virtual void update(double dt, ParticleData *p) override;
		};

		class VelColorUpdater : public ParticleUpdater
		{
		public:
			glm::vec4 m_minVel;
			glm::vec4 m_maxVel;
		public:
			VelColorUpdater() : m_minVel(0.0), m_maxVel(1.0) { }

			virtual void update(double dt, ParticleData *p) override;
		};

		class BasicTimeUpdater : public ParticleUpdater
		{
		public:
			virtual void update(double dt, ParticleData *p) override;
		};
	}
}
