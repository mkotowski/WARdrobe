#pragma once

#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"



class Renderer
{
public:
    Renderer();
    Renderer(bool);
    void Draw(Shader *, Model *, unsigned int, Camera *);    
    void Draw(Shader *, Model *, unsigned int);    
    void DrawGeometry(Shader *, unsigned int, Camera *);    
    void Init();

private:
	  unsigned int VAO, VBO, EBO, texture;
};