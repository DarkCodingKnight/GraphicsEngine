#pragma once

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "ResourceManager.hpp"

namespace Engine {

class Camera {
public:
    Camera() = delete;
    Camera(const Camera& other) = delete;
    Camera(Camera&& other) = delete;
    Camera& operator=(const Camera& other) = delete;
    Camera& operator=(Camera&& other) = delete;
    
    Camera(ResourceManager* _pResourceManager_,
           float display_width,
           float display_height,
           float z_near,
           float z_far,
           float field_of_view);
    
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    
    void setCameraSpeed(float speed) { camera_speed = speed; }
    void setMouseSensitivity(float sensitivity) { mouse_sensitivity = sensitivity; }
    void setEnableRenderTime(bool value) { isRenderTimeEnable = value; }
    
    ~Camera() { pResourceManager = nullptr; };
    
private:
    void updateCamera();
    float getRenderTime() {
        if (isRenderTimeEnable) return render_time;
        else return 1.0f;
    }
    
    bool isRenderTimeEnable = false;
    float last_render_time = 0.0f;
    float render_time = 1.0f;
    float camera_speed = 2.5f;
    
    double last_mouse_pos_x = 0.0f;
    double last_mouse_pos_y = 0.0f;
    
    float mouse_sensitivity = 0.1f;
    float yaw_angle = 0.0f;
    float pitch_angle = 0.0f;
    
    glm::vec3 cameraPosition_vec;
    glm::vec3 cameraDirection_vec;
    glm::vec3 cameraRight_vec;
    glm::vec3 cameraUp_vec;
    
    glm::mat4 view_matrix;
    glm::mat4 projection_matrix;
    
    ResourceManager* pResourceManager = nullptr;
};

}
