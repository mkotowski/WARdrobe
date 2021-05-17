#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in ivec4 aBoneIDs;
layout (location = 4) in vec4 aWeights;

const int MAX_BONES = 16;
uniform mat4 gBones[MAX_BONES];

uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{	
	mat4 boneTransform = gBones[aBoneIDs[0]] * aWeights[0];
		boneTransform += gBones[aBoneIDs[1]] * aWeights[1];
		boneTransform += gBones[aBoneIDs[2]] * aWeights[2];
		boneTransform += gBones[aBoneIDs[3]] * aWeights[3];

	vec4 PosL = boneTransform * vec4(aPos, 1.0);
	
	gl_Position = lightSpaceMatrix  * model * PosL;
}