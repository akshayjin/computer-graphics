#include "include/fountainEffect.h"
#include <cmath>
#include <iostream>

bool FountainEffect::initialize(size_t numParticles)
{
	//
	// particles
	//
	const size_t NUM_PARTICLES = numParticles == 0 ? 100 : numParticles;
	m_system = std::make_shared<particles::ParticleSystem>(NUM_PARTICLES);

	//
	// emitter:
	//
	auto particleEmitter = std::make_shared<particles::ParticleEmitter>();
	{
		particleEmitter->m_emitRate = (float)NUM_PARTICLES*0.1f;

		// pos:
		m_posGenerator = std::make_shared<particles::generators::BoxPosGen>();
		m_posGenerator->m_pos = glm::vec4{ 0.0, 0.0, 0.0, 0.0 };
		m_posGenerator->m_maxStartPosOffset = glm::vec4{ 0.0, 0.0, 0.0, 0.7 };
		particleEmitter->addGenerator(m_posGenerator);

		m_colGenerator = std::make_shared<particles::generators::BasicColorGen>();
		
		m_colGenerator->m_minStartCol = glm::vec4{ 0.7, 0.3, 0.0, 1.0 };
		m_colGenerator->m_maxStartCol = glm::vec4{ 0.9, 0.4, 0.1, 1.0 };
		m_colGenerator->m_minEndCol = glm::vec4{ 0.8, 0.8, 0.2, 0.0 };
		m_colGenerator->m_maxEndCol = glm::vec4{ 1.0, 0.9, 0.3, 0.0 };
		particleEmitter->addGenerator(m_colGenerator);

		auto velGenerator = std::make_shared<particles::generators::BasicVelGen>();
		velGenerator->m_minStartVel = glm::vec4{ -0.2f, 0.22f, -0.2f, 0.0f };
		velGenerator->m_maxStartVel = glm::vec4{ 0.2f, 0.65f, 1.5f, 0.0f };
		particleEmitter->addGenerator(velGenerator);

		auto timeGenerator = std::make_shared<particles::generators::BasicTimeGen>();
		timeGenerator->m_minTime = 3.0f;
		timeGenerator->m_maxTime = 8.0f;
		particleEmitter->addGenerator(timeGenerator);
	}
	m_system->addEmitter(particleEmitter);

	auto timeUpdater = std::make_shared<particles::updaters::BasicTimeUpdater>();
	m_system->addUpdater(timeUpdater);

	auto colorUpdater = std::make_shared<particles::updaters::BasicColorUpdater>();
	m_system->addUpdater(colorUpdater);

	m_eulerUpdater = std::make_shared<particles::updaters::EulerUpdater>();
	m_eulerUpdater->m_globalAcceleration = glm::vec4{ 0.0, -15.0, -2.0, 0.0 };
	m_system->addUpdater(m_eulerUpdater);

	m_floorUpdater = std::make_shared<particles::updaters::FloorUpdater>();
	m_system->addUpdater(m_floorUpdater);

	m_objectCollisionUpdater = std::make_shared<particles::updaters::ObjectCollisionUpdater>();
	m_system->addUpdater(m_objectCollisionUpdater);

	return true;
}

bool FountainEffect::initializeRenderer()
{
	m_renderer = particles::RendererFactory::create("gl");
	m_renderer->generate(m_system.get(), false);

	return true;
}

void FountainEffect::clean()
{
	if (m_renderer) m_renderer->destroy();
}

void FountainEffect::update(double dt)
{
	static double time = 0.0;
	time += dt;

	m_posGenerator->m_pos.x = 0.05f*sin((float)time*2.5f);
	m_posGenerator->m_pos.z = 0.05f*cos((float)time*2.5f);
}

void FountainEffect::cpuUpdate(double dt)
{
	m_system->update(dt);
}

void FountainEffect::render()
{
	m_renderer->render();
}
