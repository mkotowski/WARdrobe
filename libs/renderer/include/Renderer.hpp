#pragma once

#include "Model.hpp"
#include "Shader.hpp"



class Renderer
{
public:
    Renderer();
    void Draw(Shader *, Model *, unsigned int);    
    void Init();

private:
    
};