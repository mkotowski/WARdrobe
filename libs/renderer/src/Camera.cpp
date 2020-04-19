#include "Camera.hpp"

Camera::Camera()
{

}

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float fov)
{
    this->cameraPos = position;
    this->cameraFront = front;
    this->cameraUp = up;
    this->fieldOfView = fov;
}

