#include "Camera.hpp"

namespace Engine {

Camera::Camera(ResourceManager* _pResourceManager_,
               float display_width,
               float display_height,
               float z_near,
               float z_far,
               float field_of_view) : pResourceManager(_pResourceManager_)
{
    cameraPosition_vec = glm::vec3(0.0f, 0.0f, 3.0f);
    cameraDirection_vec = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp_vec = glm::vec3(0.0f, 1.0f, 0.0f);
    
    view_matrix = glm::lookAt(cameraPosition_vec,
                              cameraDirection_vec + cameraPosition_vec,
                              cameraUp_vec);
    
    projection_matrix = glm::perspective(glm::radians(field_of_view),
                                         display_width / display_height,
                                         z_near,
                                         z_far / z_near);
}

glm::mat4 Camera::getViewMatrix() {
    float currentRenderTime = glfwGetTime();
    render_time = currentRenderTime - last_render_time;
    last_render_time = currentRenderTime;
    
    updateCamera();
    
    view_matrix = glm::lookAt(cameraPosition_vec, cameraDirection_vec + cameraPosition_vec, cameraUp_vec);
    
    return view_matrix;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projection_matrix;
}

void Camera::updateCamera() {
    if (pResourceManager->getKeyPressed(87)) {
        cameraPosition_vec += camera_speed * getRenderTime() * cameraDirection_vec;
    }
    if (pResourceManager->getKeyPressed(83)) {
        cameraPosition_vec -= camera_speed * getRenderTime() * cameraDirection_vec;
    }
    if (pResourceManager->getKeyPressed(65)) {
        cameraPosition_vec -= camera_speed * getRenderTime() * (glm::normalize(glm::cross(cameraDirection_vec, cameraUp_vec)));
    }
    if (pResourceManager->getKeyPressed(68)) {
        cameraPosition_vec += camera_speed * getRenderTime() * (glm::normalize(glm::cross(cameraDirection_vec, cameraUp_vec)));
    }
    
    double cursor_offset_x = pResourceManager->getCursorOffsetX();
    double cursor_offset_y = pResourceManager->getCursorOffsetY();
    
    yaw_angle += cursor_offset_x * mouse_sensitivity;
    pitch_angle += cursor_offset_y * mouse_sensitivity;
    
    if (pitch_angle > 89.0f) pitch_angle = 89.0f;
    if (pitch_angle < -89.0f) pitch_angle = -89.0f;
    
    std::cout << "Yaw angle: " << yaw_angle << "Pitch angle: " << pitch_angle << std::endl;
    std::cout << "Cursor position X: " << pResourceManager->getCursorPositionX()
    << "Cursor position Y: " << pResourceManager->getCursorPositionY() << std::endl;
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw_angle) * cos(glm::radians(pitch_angle)));
    direction.y = sin(glm::radians(pitch_angle));
    direction.z = sin(glm::radians(yaw_angle) * cos(glm::radians(pitch_angle)));
    cameraDirection_vec = glm::normalize(direction);
}

}
