#ifndef ANIMATION_H
#define ANIMATION_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>


class Animation
{
public:
    const aiScene* scene;
    float ticksPerSecond;
    std::string animationName;
    bool isLooping;
    Animation()
    {

    }

    Animation(const aiScene* scene, float ticksPerSecond, std::string animationName, bool isLooping)
    {
        this->scene = scene;
        this->ticksPerSecond = ticksPerSecond;
        this->animationName = animationName;
        this->isLooping = isLooping;
    }
};

#endif