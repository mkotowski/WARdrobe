#pragma once
#include <glm/glm.hpp>


class Camera
{
public:
    Camera();
    Camera(glm::vec3, glm::vec3, glm::vec3, float);
    glm::vec3 cameraPos;
    glm::vec3 cameraFront;
    glm::vec3 cameraUp;
    float fieldOfView;
};