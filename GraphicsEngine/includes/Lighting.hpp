#pragma once

#include "GraphicsObject.hpp"
#include "Camera.hpp"

namespace Engine {

class Lighting : public GraphicsObject {
public:
    Lighting(const Lighting& other) = delete;
    Lighting(Lighting&& other) = delete;
    Lighting& operator=(const Lighting& other) = delete;
    Lighting& operator=(Lighting&& other) = delete;
    
    Lighting();
    
    ~Lighting();
    
private:
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPosition = glm::vec3(0.0f, 3.0f, 0.0f);
};

}
