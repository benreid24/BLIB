#ifndef TRANSFORMUNIFORM_HPP
#define TRANSFORMUNIFORM_HPP

#include <glm/glm.hpp>

struct TransformUniform {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

#endif
