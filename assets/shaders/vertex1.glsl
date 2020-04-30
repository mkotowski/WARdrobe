#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// GLSL Hacker automatic uniforms:
uniform mat4 ModelViewMatrix;

void main()
{
	mat4 modelView = ModelViewMatrix;

	// First colunm.
  modelView[0][0] = 1.0; 
  modelView[0][1] = 0.0; 
  modelView[0][2] = 0.0; 

    // Second colunm.
    modelView[1][0] = 0.0; 
    modelView[1][1] = 1.0; 
    modelView[1][2] = 0.0; 

  // Thrid colunm.
  modelView[2][0] = 0.0; 
  modelView[2][1] = 0.0; 
  modelView[2][2] = 1.0; 
  
  FragPos = vec3(model * vec4(aPos, 1.0f));
  Normal = mat3(transpose(inverse(model))) * aNormals;
  TexCoord = vec2(aTexCoord.x, aTexCoord.y);
  vec4 P = modelView * vec4(aPos, 1.0f);
  gl_Position = projection * P;

	
}