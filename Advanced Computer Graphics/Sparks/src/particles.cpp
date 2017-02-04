#include "include/particles.h"
#include <assert.h>
#include <algorithm>
#include <iostream>

namespace particles
{
	void ParticleData::generate(size_t maxSize)
	{
		m_count = maxSize;
		m_countAlive = 0;

		m_pos.reset(new glm::vec4[maxSize]);
		m_col.reset(new glm::vec4[maxSize]);
		m_startCol.reset(new glm::vec4[maxSize]);
		m_endCol.reset(new glm::vec4[maxSize]);
		m_vel.reset(new glm::vec4[maxSize]);
		m_acc.reset(new glm::vec4[maxSize]);
		m_time.reset(new glm::vec4[maxSize]);
		m_alive.reset(new bool[maxSize]);
	}

	void ParticleData::kill(size_t id)
	{
		{
			m_alive[id] = false;
			swapData(id, m_countAlive - 1);
			m_countAlive--;
		}
	}

	void ParticleData::wake(size_t id)
	{
		{
			m_alive[id] = true;
			m_countAlive++;
		}
	}

	void ParticleData::swapData(size_t a, size_t b)
	{
		m_pos[a] = m_pos[b];
		m_col[a] = m_col[b];
		m_startCol[a] = m_startCol[b];
		m_endCol[a] = m_endCol[b];
		m_vel[a] = m_vel[b];
		m_acc[a] = m_acc[b];
		m_time[a] = m_time[b];
	}

	void ParticleData::copyOnlyAlive(const ParticleData *source, ParticleData *destination)
	{
		assert(source->m_count == destination->m_count);

		size_t id = 0;
		for (size_t i = 0; i < source->m_countAlive; ++i)
		{
			//if (source->m_alive[i])
			{
				destination->m_pos[id] = source->m_pos[i];
				destination->m_col[id] = source->m_col[i];
				destination->m_startCol[id] = source->m_startCol[i];
				destination->m_endCol[id] = source->m_endCol[i];
				destination->m_vel[id] = source->m_vel[i];
				destination->m_acc[id] = source->m_acc[i];
				destination->m_time[id] = source->m_time[i];
				destination->m_alive[id] = true;
				id++;
			}
		}

		destination->m_countAlive = id;
	}

	size_t ParticleData::computeMemoryUsage(const ParticleData &p)
	{
		return p.m_count * (7 * sizeof(glm::vec4)+sizeof(bool)) + sizeof(size_t)* 2;
	}

	// ParticleEmitter class 

	void ParticleEmitter::emit(double dt, ParticleData *p)
	{
		const size_t maxNewParticles = static_cast<size_t>(dt*m_emitRate);
		const size_t startId = p->m_countAlive;
		const size_t endId = std::min(startId + maxNewParticles, p->m_count-1);

		for (auto &gen : m_generators)
			gen->generate(dt, p, startId, endId);

		for (size_t i = startId; i < endId; ++i)
		{
			p->wake(i);
		}
	}

	// ParticleSystem class 

	ParticleSystem::ParticleSystem(size_t maxCount)
	{
		m_count = maxCount;
		m_particles.generate(maxCount);
		m_aliveParticles.generate(maxCount);

		for (size_t i = 0; i < maxCount; ++i)
			m_particles.m_alive[i] = false;
	}

	void ParticleSystem::update(double dt)
	{
		for (auto & em : m_emitters)
		{
			em->emit(dt, &m_particles);
		}

		for (size_t i = 0; i < m_count; ++i)
		{
			m_particles.m_acc[i] = glm::vec4(0.0f);
		}

		for (auto & up : m_updaters)
		{
			up->update(dt, &m_particles);
		}

		ParticleData::copyOnlyAlive(&m_particles, &m_aliveParticles);
	}

	void ParticleSystem::reset()
	{
		m_particles.m_countAlive = 0;
	}

	size_t ParticleSystem::computeMemoryUsage(const ParticleSystem &p)
	{
		return 2 * ParticleData::computeMemoryUsage(p.m_particles);
	}
}
