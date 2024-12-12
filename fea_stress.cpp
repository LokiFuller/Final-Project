#include "fea_stress.h"
#include <iostream>
#include <algorithm>

// material properties and constants
const float YOUNGS_MODULUS = 1.3e+6;  // Pa (Young's Modulus for wood)
const float DENSITY = 510;          // kg/m^3 (density of wood)
const float GRAVITY = 9.81;          // m/s^2 (gravitational acceleration)
const float THICKNESS = 0.01;        // m (thickness for all elements)

// calculate nodal forces based on mass distribution
void calculateNodalForces(std::vector<float>& nodal_forces, const std::vector<Element>& elements, const std::vector<Node>& nodes) {
    // calculate the total force acting on the structure
    float total_weight = 0.0f;
    for (const auto& element : elements) {
        float volume = element.area * THICKNESS;
        total_weight += DENSITY * GRAVITY * volume;
    }

    // apply forces to each node, considering that nodes near the bottom are more heavily loaded
    for (size_t i = 0; i < nodes.size(); ++i) {
        float distance_from_base = glm::length(nodes[i].position); // Assuming base is at origin (0,0,0)
        float weight_factor = 1.0f / (distance_from_base + 1.0f);  // weight factor based on distance from base

        // distribute the weight to nodes
        for (const auto& element : elements) {
            if (std::find(element.node_ids.begin(), element.node_ids.end(), i) != element.node_ids.end()) {
                nodal_forces[i] += (total_weight * weight_factor) / 3.0f;
            }
        }
    }
}

// calculate stress for each node based on displacement
void calculateStress(const std::vector<float>& nodal_forces, const std::vector<Node>& nodes, const std::vector<Element>& elements, std::vector<float>& stresses) {
    std::cout << "\nStress Calculations:\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        // sum of areas connected to the node
        float area_sum = 0.0f;
        for (const auto& element : elements) {
            if (std::find(element.node_ids.begin(), element.node_ids.end(), i) != element.node_ids.end()) {
                area_sum += element.area;
            }
        }

        // stress calculation: stress = force / area
        stresses[i] = (area_sum > 0.0f) ? nodal_forces[i] / area_sum : 0.0f;

        // display output for each node
        std::cout << "Node ID: " << nodes[i].id
            << ", Force: " << nodal_forces[i]
            << ", Area Sum: " << area_sum
            << ", Stress: " << stresses[i] << " Pa\n";
    }
}