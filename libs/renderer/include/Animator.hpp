#ifndef ANIMATOR_H
#define ANIMATOR_H
#include "Model.hpp"
#include "Bone.hpp"
#include "Animation.hpp"
#include <string>
#include <glm/glm.hpp>

class Animator
{
public:

    Model* animatedModel;
    Animation* currentAnimation;
    double animationTime = 0.0;
    Animator()
    {

    }
    /**
     * Konstruktor, przypisuje wskaźnik do modelu
     */
    Animator(Model* animatedModel)
    {
        this->animatedModel = animatedModel;
    }

    // Ustawia animację do odegrania
    void PlayAnimation(Animation* animation)
    {
        this->animationTime = 0;
        this->currentAnimation = animation;
    }

    // Aktualizuje animację oraz aktualizuje pozycje kości
    void Update(double dt)
    {
        if (currentAnimation == nullptr)
        {
            return;
        }
        
        IncreaseAnimationTime(dt);
        std::map<string, glm::mat4> currentPose = CalculateCurrentAnimationPose();
        ApplyPoseToBones(currentPose, animatedModel->rootBone, glm::mat4(1.0f));
    }

    // Zwiększa czas i zapętla animację
    void IncreaseAnimationTime(double dt)
    {
        this->animationTime += dt;
        if (animationTime >= currentAnimation->length)
        {
            animationTime = 0;
        }
    }

    // Oblicza transformy dla wszystkich kości dla momentu pomiędzy klatkami 
    std::map<std::string, glm::mat4> CalculateCurrentAnimationPose()
    {
        std::vector<KeyFrame> frames = GetPreviousAndNextFrames();
        double progression = CalculateProgression(frames.at(0), frames.at(1));
        return interpolatePoses(frames.at(0), frames.at(1), progression);
    }

    // Modyfikuje dla kości i dla jej dzieci animatedTransform, który jest obliczany
    // nazwa funkcji na oryginalnym repo - ApplyPoseToJoint (chyba)
    void ApplyPoseToBones(std::map<std::string, glm::mat4> currentPose, Bone* bone, glm::mat4 parentTransform)
    {
        glm::mat4 currentLocalTransform = currentPose.at(bone->name);
        glm::mat4 currentTransform = parentTransform * currentLocalTransform;
        for (auto& child : bone->children)
        {
            ApplyPoseToBones(currentPose, child, currentTransform);
        }
        currentTransform *= bone->GetInverseBindTransform();
        bone->SetAnimationTransform(currentTransform);
    }

    // Pobiera wektor 2 klatek pomiędzy którymi znajduje się teraz animacja
    std::vector<KeyFrame> GetPreviousAndNextFrames()
    {
        std::vector<KeyFrame> allFrames = currentAnimation->keyFrames;
        KeyFrame previousFrame = allFrames.at(0);
        KeyFrame nextFrame = allFrames.at(0);
        for (int i = 1; i < allFrames.size(); i++)
        {
            nextFrame = allFrames.at(i);
            if (nextFrame.timeStamp > animationTime)
            {
                break;
            }
            previousFrame = allFrames.at(i);
        }
        return std::vector<KeyFrame> {previousFrame, nextFrame};
    }

    // oblicza moment, w którym znajduje się obecnie animacja
    double CalculateProgression(KeyFrame previousFrame, KeyFrame nextFrame)
    {
        double totalTime = nextFrame.timeStamp - previousFrame.timeStamp;
        double currentTime = animationTime - previousFrame.timeStamp;
        return currentTime / totalTime;
    }
    
    // Interpolacja pomiędzy dwoma pozami
    std::map<std::string, glm::mat4> interpolatePoses(KeyFrame previousFrame, KeyFrame nextFrame, double progression)
    {
        std::map<std::string, glm::mat4> currentPose;
        for (auto const& [key, val] : previousFrame.pose)
        {

            BoneTransform previousTransform = previousFrame.pose.at(key);

            BoneTransform nextTransform = nextFrame.pose.at(key);
            
            BoneTransform currentTransform = BoneTransform();
            
            currentTransform.interpolate(previousTransform, nextTransform, progression);
                        
            glm::mat4 localTransform = currentTransform.GetLocalTransform();

            currentPose.insert(std::pair<string, glm::mat4>(key, localTransform));
            
        }
        return currentPose;
    }


};

#endif