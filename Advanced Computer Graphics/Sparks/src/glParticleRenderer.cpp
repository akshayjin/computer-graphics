#include "include/glParticleRenderer.h"
#include "include/particles.h"
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
#include <glm/vec4.hpp>

#include <assert.h>
#include <iostream>
#include <memory>

namespace particles
{

	void GLParticleRenderer::generate(ParticleSystem *sys, bool)
	{
		assert(sys != nullptr);
		m_system = sys;
	}

	void GLParticleRenderer::destroy()
	{
		if (m_bufPos != 0)
		{
			glDeleteBuffers(1, &m_bufPos);
			m_bufPos = 0;
		}

		if (m_bufCol != 0)
		{
			glDeleteBuffers(1, &m_bufCol);
			m_bufCol = 0;
		}
	}

	void GLParticleRenderer::update()
	{
		assert(m_system != nullptr);
		assert(m_bufPos > 0 && m_bufCol > 0);
	}

	void GLParticleRenderer::render()
	{
		const size_t count = m_system->numAliveParticles();
		float *ptr_v = (float *)(m_system->finalData()->m_pos.get());
		float *ptr_c = (float *)(m_system->finalData()->m_col.get());
		
		glEnableClientState(GL_VERTEX_ARRAY);	
		glEnableClientState( GL_COLOR_ARRAY );
			glVertexPointer(4,GL_FLOAT, 0, ptr_v);	
			glColorPointer(4,GL_FLOAT,0, ptr_c);	
			glPointSize(2);
			glDrawArrays(GL_POINTS, 0, count);	
		glDisableClientState(GL_VERTEX_ARRAY);    
		glDisableClientState( GL_COLOR_ARRAY );
	}
}
