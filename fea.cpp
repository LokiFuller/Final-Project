#include "fea.h"
#include <iostream>
#include <glm.hpp>

// global variables
std::vector<Node> nodes;
std::vector<Element> elements;

// calculate triangle area
float calculateTriangleArea(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
    glm::vec3 ab = b - a;
    glm::vec3 ac = c - a;
    return 0.5f * glm::length(glm::cross(ab, ac));
}

// initialize FEA nodes and elements
void initializeFEA(const std::vector<glm::vec3>& vert_positions) {
    for (size_t i = 0; i < vert_positions.size(); ++i) {
        nodes.push_back({ vert_positions[i], static_cast<int>(i) });
    }

    for (size_t i = 0; i < vert_positions.size(); i += 3) {
        if (i + 2 < vert_positions.size()) {
            int id1 = static_cast<int>(i);
            int id2 = static_cast<int>(i + 1);
            int id3 = static_cast<int>(i + 2);
            float area = calculateTriangleArea(vert_positions[id1], vert_positions[id2], vert_positions[id3]);
            elements.push_back({ {id1, id2, id3}, area });
        }
    }

    std::cout << "FEA Initialization Complete.\n";
    std::cout << "Number of Nodes: " << nodes.size() << "\n";
    std::cout << "Number of Elements: " << elements.size() << "\n";
}

// print FEA details
void printFEAInfo() {
    std::cout << "\nFEA Nodes:\n";
    for (const auto& node : nodes) {
        std::cout << "Node ID: " << node.id << ", Position: ("
            << node.position.x << ", " << node.position.y << ", " << node.position.z << ")\n";
    }

    std::cout << "\nFEA Elements:\n";
    for (const auto& element : elements) {
        std::cout << "Element Nodes: ["
            << element.node_ids[0] << ", "
            << element.node_ids[1] << ", "
            << element.node_ids[2] << "], Area: "
            << element.area << " m^2\n";
    }
}
