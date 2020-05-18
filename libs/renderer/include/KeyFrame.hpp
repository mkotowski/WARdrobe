#ifndef KEYFRAME_H
#define KEYFRAME_H

#include <map>
#include <string>
#include <BoneTransform.hpp>

class KeyFrame
{
public:
    // ALL ANIMATIONS REQUIRE TO HAVE KEYFRAMES AT THE SAME TIMESTAMP!!
    double timeStamp;
    // name of the bone and transform for it
    std::map<std::string, BoneTransform> pose;
    KeyFrame(float timeStamp, std::map<std::string, BoneTransform> boneKeyFrames)
    {
        this->timeStamp = timeStamp;
        this->pose = boneKeyFrames;
    }
    KeyFrame()
    {
        
    }
};

#endif