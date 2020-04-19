#pragma once

#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"



class Renderer
{
public:
    Renderer();
    void Draw(Shader *, Model *, unsigned int, Camera *);    
    void Draw(Shader *, Model *, unsigned int);    
    void Init();

private:
    
};