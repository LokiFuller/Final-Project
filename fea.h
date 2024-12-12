#pragma once
#include <vector>
#include <glm.hpp>

// declarations
struct Node {
    glm::vec3 position;
    int id;
};

struct Element {
    std::vector<int> node_ids;
    float area;
};

// global variables
extern std::vector<Node> nodes;
extern std::vector<Element> elements;

// functions
void initializeFEA(const std::vector<glm::vec3>& vert_positions);
void printFEAInfo();
float calculateTriangleArea(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);