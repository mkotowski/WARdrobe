#ifndef BONE_H
#define BONE_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <iostream>

class Bone
{
public:
    int ID;
    std::string name;
    std::vector<Bone*> children;
    glm::mat4 animatedTransform;
    glm::mat4 localBindTransform;
    glm::mat4 inverseBindTransform;
    glm::mat4 offsetMatrix;
    std::string parentBone;

    Bone();
    Bone(int index, std::string name, glm::mat4 bindLocalTransform, glm::mat4 offsetMatrix);
    void AddChild(Bone* bone);
    glm::mat4 GetAnimatedTransform();
    void SetAnimationTransform(glm::mat4 animationTransform);
    glm::mat4 GetInverseBindTransform();
    void CalcInverseBindTransform(glm::mat4 parentBindTransform);
};

static aiMatrix4x4 GLMMat4ToAi(glm::mat4 mat)
{
    return aiMatrix4x4(mat[0][0], mat[0][1], mat[0][2], mat[0][3],
                       mat[1][0], mat[1][1], mat[1][2], mat[1][3],
                       mat[2][0], mat[2][1], mat[2][2], mat[2][3],
                       mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
                       
};

static glm::mat4 AiToGLMMat4(aiMatrix4x4& in_mat)
{
    glm::mat4 tmp;
    tmp[0][0] = in_mat.a1;
    tmp[1][0] = in_mat.b1;
    tmp[2][0] = in_mat.c1;
    tmp[3][0] = in_mat.d1;

    tmp[0][1] = in_mat.a2;
    tmp[1][1] = in_mat.b2;
    tmp[2][1] = in_mat.c2;
    tmp[3][1] = in_mat.d2;

    tmp[0][2] = in_mat.a3;
    tmp[1][2] = in_mat.b3;
    tmp[2][2] = in_mat.c3;
    tmp[3][2] = in_mat.d3;

    tmp[0][3] = in_mat.a4;
    tmp[1][3] = in_mat.b4;
    tmp[2][3] = in_mat.c4;
    tmp[3][3] = in_mat.d4;
  
    return tmp;
};

static void PrintMat4(glm::mat4 mat)
{
    std::cout << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << " " << mat[0][3] << std::endl;
    std::cout << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << " " << mat[1][3] << std::endl;
    std::cout << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << " " << mat[2][3] << std::endl;
    std::cout << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << std::endl;
}

#endif