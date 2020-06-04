#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

out vec4 Weights;
out vec4 BoneIDs;

const int MAX_BONES = 16;
uniform mat4 gBones[MAX_BONES];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0f));
	Normal = mat3(transpose(inverse(model))) * aNormals;
	TexCoord = aTexCoord;
	
	mat4 boneTransform = gBones[aBoneIDs[0]] * aWeights[0];
		boneTransform += gBones[aBoneIDs[1]] * aWeights[1];
		boneTransform += gBones[aBoneIDs[2]] * aWeights[2];
		boneTransform += gBones[aBoneIDs[3]] * aWeights[3];

	vec4 PosL = boneTransform * vec4(aPos, 1.0);


	Weights = aWeights;
	BoneIDs = aBoneIDs;

	gl_Position = projection * view  * model * PosL;
}