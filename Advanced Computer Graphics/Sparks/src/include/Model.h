#pragma once

#include <vector>
#include <string>
#include "PLYModel.h"
#include "glm.h"
#include "particles.h"
#include "effect.h"
#include <chrono>

class CpuTimeQuery
{
protected:
	double m_time;

protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_cpuTimePointStart;

public:
	void begin()
	{
		m_cpuTimePointStart = std::chrono::high_resolution_clock::now();
	}

	void end()
	{
		auto diff = std::chrono::high_resolution_clock::now() - m_cpuTimePointStart;
		auto mili = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();
		m_time = 0.5*(m_time + (double)mili);
	}

	double timeInMilisec() const { return m_time; }
	double timeInSeconds() const { return m_time*0.001; }
};

class Model
{
public:	
	void init();
	PLYModel model3D_1;
	PLYModel model3D_2;
	GLMmodel* sawblade;
	GLMmodel* slab;
	GLMmodel* cube;
	GLMmodel* sphere;
	GLMmodel* cone;
	GLMmodel* bunny;
	void render();
	void drawAxis();
	void drawFloor();
	void drawSawBlade();
	void drawSlab();
	void drawParticles();
	void drawObjects();
	void rotate(float deg);
	void scale(float times);
	void translate(float x, float y, float z);
	GLuint loadBMP_custom(const char * imagepath);
	std::vector<std::string> texture_files;
	std::vector<GLuint> texture_ids;
	int model3D_1_texture = 5;
	int model3D_2_texture = 2;
	int saw_rotation = 0;
	float transx=0.0;
	float transy=0.0;
	float transz=0.0;
	
	//~  --------------------------------Particles-------------------------------------------------------------
	const std::vector<std::string> EFFECTS_NAME{ "fountain" };
	const size_t START_NUM_PARTICLES{ 35000 };

	const double DELTA_TIME{ 1.0 / 50.0 };	// 60 fps
	const size_t FRAME_COUNT{ 200 };
	size_t step = 0;
	
	std::shared_ptr<IEffect> gCurrentEffect;

	CpuTimeQuery timer;
};
