#pragma once
#include <glm/glm.hpp>

// Add all contact forces

// Environment Forces
const glm::vec4 GRAVITY (0, -9.8, 0, 0);
const glm::vec4 WIND (0, 0, 0, 0);

const float VISCOUS_DAMPING_COEFF = 0.5; // Kd
const float VISCOUS_DAMPING_COEFF_TANGENT = 0.5; // Kt
const float ELASTIC_RESTORATION_COEFF = 50; // Kr
const float RESTITUTION_COEFF = 0.1; // epsilon
const float FRICTION_COEFF = 0.1; // mu
const float AIR_RESISTANCE = 0.995; // mu
const float GROUND_FRICTION = 0.75; // mu
