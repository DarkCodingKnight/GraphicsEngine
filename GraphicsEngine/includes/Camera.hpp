#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Engine {

class Camera {
public:
    Camera();
    
    
    
    ~Camera() = default;
    
private:
    glm::vec3 cameraPosition;
    glm::vec3 cameraTarget;
    glm::vec3 cameraDirection;
    
    glm::vec3 cameraRight;
    glm::vec3 cameraUp;
    
    glm::mat4 view;
};

}
