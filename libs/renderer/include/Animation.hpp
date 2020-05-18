#ifndef ANIMATION_H
#define ANIMATION_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <KeyFrame.hpp>

class Animation
{
public:
    std::string name;
    double length;
    std::vector<KeyFrame> keyFrames;

    Animation()
    {

    }
    /**
     * Konstruktor, długość w sekundach (0.833 sekundy np.), wektor KeyFrames
     */
    Animation(double lengthInSeconds, std::vector<KeyFrame> keyframes)
    {
        this->keyFrames = keyFrames;
        this->length = lengthInSeconds;
    }
};

#endif