#ifndef BONETRANSFORM_H
#define BONETRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

class BoneTransform
{
public:

    glm::vec3 position;
    glm::quat rotation;

    BoneTransform()
    {

    }

    BoneTransform(glm::vec3 position, glm::quat rotation)
    {
        this->position = position;
        this->rotation = rotation;
    }

    glm::mat4 GetLocalTransform()
    {
        glm::mat4 mat = glm::mat4(1.0f);
        mat = glm::translate(mat, position);
        mat *= glm::toMat4(rotation);
        return mat;
    }

    // interpoluje BoneTransform pomiędzy klatkami 
    void interpolate (BoneTransform frameA, BoneTransform frameB, float progression)
    {
        glm::vec3 pos = glm::mix(frameA.position, frameB.position, progression);
        glm::quat rot = glm::slerp(frameA.rotation, frameB.rotation, progression);

        this->position = pos;
        this->rotation = rot;
        //return BoneTransform(pos, rot);
    }
    

};

#endif