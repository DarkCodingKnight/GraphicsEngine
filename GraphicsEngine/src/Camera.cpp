#include "Camera.hpp"

namespace Engine {

Camera::Camera(ResourceManager* _pResourceManager_,
               float windowWidth,
               float windowHeight,
               float zNear,
               float zFar,
               float fieldOfView) :
pResourceManager(_pResourceManager_), window_width(windowWidth),
window_height(windowHeight), z_near(zNear), z_far(zFar), field_of_view(fieldOfView)
{
    setProjectionMatrix(window_width, window_height, z_near, z_far, field_of_view);
}

glm::mat4 Camera::getViewMatrix() {
    float currentRenderTime = glfwGetTime();
    render_time = currentRenderTime - last_render_time;
    last_render_time = currentRenderTime;
    
    updateCamera();
    
    view_matrix = glm::lookAt(cameraPosition_vec, cameraDirection_vec + cameraPosition_vec, cameraUp_vec);
    
    return view_matrix;
}

void Camera::setProjectionMatrix(float windowWidth,
                                 float windowHeight,
                                 float zNear,
                                 float zFar,
                                 float fieldOfView)
{
    projection_matrix = glm::perspective(glm::radians(fieldOfView),
                                         windowWidth / windowHeight,
                                         zNear,
                                         zFar / zNear);
}

void Camera::setProjectionMatrix(float windowWidth,
                                 float windowHeight)
{
    projection_matrix = glm::perspective(glm::radians(field_of_view),
                                         window_width / window_height,
                                         z_near,
                                         z_far / z_near);
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
    
    if (pResourceManager->getMouseKeyPressed(0)) updateDirection();
}

void Camera::updateDirection() {
    double cursor_offset_x = pResourceManager->getCursorOffsetX();
    double cursor_offset_y = pResourceManager->getCursorOffsetY();
    
    yaw_angle += cursor_offset_x * mouse_sensitivity;
    pitch_angle -= cursor_offset_y * mouse_sensitivity;
    
    if (pitch_angle > 89.0f) pitch_angle = 89.0f;
    if (pitch_angle < -89.0f) pitch_angle = -89.0f;
    
    std::string message = "Yaw angle: " + std::to_string(yaw_angle) + ", Pitch angle: " + std::to_string(pitch_angle);
    ConsoleText::printBlue(message, "Camera");
    
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw_angle)) * cos(glm::radians(pitch_angle));
    direction.y = sin(glm::radians(pitch_angle));
    direction.z = sin(glm::radians(yaw_angle)) * cos(glm::radians(pitch_angle));
    cameraDirection_vec = glm::normalize(direction);
    
    cameraRight_vec = glm::normalize(glm::cross(worldUp_vec, cameraDirection_vec));
    cameraUp_vec = glm::normalize(glm::cross(cameraRight_vec, cameraDirection_vec));
    cameraUp_vec *= -1;
}

}
