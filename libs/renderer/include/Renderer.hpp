#pragma once

#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"


class Renderer
{
public:
    Renderer();
    void Draw(Shader *, Model *, Camera *, glm::vec3, glm::vec3, glm::vec3, int , int);    
    //void Draw(Shader *, Model *, unsigned int);    
    void Init();

private:
    
};