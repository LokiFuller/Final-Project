#pragma once
#include <vector>
#include <glm.hpp>
#include "fea.h"

// material properties
extern const float YOUNGS_MODULUS;  // Pa
extern const float DENSITY;          // kg/m^3
extern const float GRAVITY;          // m/s^2
extern const float THICKNESS;        // m (assumed thickness)

// calculate nodal forces based on material properties and geometry
void calculateNodalForces(std::vector<float>& nodal_forces, const std::vector<Element>& elements, const std::vector<Node>& nodes);

// calculate stress for each node
void calculateStress(const std::vector<float>& nodal_forces, const std::vector<Node>& nodes, const std::vector<Element>& elements, std::vector<float>& stresses);
