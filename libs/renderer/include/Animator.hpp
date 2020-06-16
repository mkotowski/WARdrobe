#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Model.hpp"
#include "Animation.hpp"
#include <map>

class Animator
{
public:
    Model* animatedModel;
    /**
     *  Contains animations and get them by the name
     */
    std::map<std::string, Animation> animationsMap;
    std::string currentAnimationName;
    std::string idleAnimationName;
    float currentAnimationTime = 0;

    Animator()
    {

    }

    // Maybe set it after creating?
    // Needs to be secured tho
    Animator(std::vector<std::pair<std::string, bool>> animations, std::vector<std::string> animationNames, std::string idleAnimationName)
    {
        int iterator = 0;
        for (auto animation : animations)
        {
            LoadAnimationFromFile(animation.first, animation.second, animationNames.at(iterator));
            iterator++;
        }

        this->currentAnimationName = animationsMap.begin()->first;
        this->idleAnimationName = idleAnimationName;
    }

    void LoadAnimationFromFile(std::string path, bool isLooping, std::string name)
    {

        Assimp::Importer importer;
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        std::string animationName = name;
        float ticksPerSecond = 25.0f;

        if (importer.GetScene()->mAnimations[0]->mTicksPerSecond != 0.0)
        {
            ticksPerSecond = importer.GetScene()->mAnimations[0]->mTicksPerSecond;
        }


        animationsMap.insert(std::pair<std::string, Animation>(animationName, Animation(importer.GetOrphanedScene(), ticksPerSecond, animationName, isLooping)));

        // check for errors
        if(!animationsMap.at(animationName).scene || animationsMap.at(animationName).scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !animationsMap.at(animationName).scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        
    }

    void PlayCurrentAnimation(float dt)
    {
        BoneTransform((double)dt, animatedModel->transforms, animationsMap.at(currentAnimationName));
        SetBoneTransforms();
        
    }

    void ChangeAnimation(std::string animationName)
    {
        if (animationsMap.at(animationName).scene)
        {
            this->currentAnimationName = animationName;
            // Resetting animation time
            this->currentAnimationTime = 0;
        }
        else
        {
            std::cout << "There is no animation with given name" << std::endl;
        }
        
    }

private:
    void CheckIfEnded()
    {
        if (!animationsMap.at(currentAnimationName).isLooping && this->currentAnimationTime >= animationsMap.at(currentAnimationName).scene->mAnimations[0]->mDuration)
        {
            this->currentAnimationName = this->idleAnimationName;
            this->currentAnimationTime = 0.0f;
        }
    }

    void SetBoneTransforms()
    {
        for (unsigned int i = 0; i < animatedModel->transforms.size(); i++)
        {
            glUniformMatrix4fv(animatedModel->mBoneLocation[i], 1, GL_TRUE, (const GLfloat*)&animatedModel->transforms[i]);
        }
    }

    unsigned int FindPosition(float pAnimationTime, const aiNodeAnim* pNodeAnim)
    {
        for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
        {
            if (pAnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
            {
                return i;
            }
        }
        assert(0);
        return 0;
    }

    // To animator/animation
    unsigned int FindRotation(float pAnimationTime, const aiNodeAnim* pNodeAnim)
    {
        for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
        {
            if (pAnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
            {
                return i;
            }
        }
        assert(0);
        return 0;
    }

    // To animator/animation
    unsigned int FindScaling(float pAnimationTime, const aiNodeAnim* pNodeAnim)
    {
        for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
        {
            if (pAnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
            {
                return i;
            }
        }
        assert(0);
        return 0;
    }


    aiVector3D CalcInterpolatedPosition(float pAnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumPositionKeys == 1)
        {
            return pNodeAnim->mPositionKeys[0].mValue;
        }

        unsigned int positionIndex = FindPosition(pAnimationTime, pNodeAnim);
        unsigned int nextPositionIndex = positionIndex + 1;
        assert(nextPositionIndex < pNodeAnim->mNumPositionKeys);

        float deltaTime = (float)(pNodeAnim->mPositionKeys[nextPositionIndex].mTime - pNodeAnim->mPositionKeys[positionIndex].mTime);
        float factor = (pAnimationTime - (float)pNodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;
        assert(factor >= 0.0f && factor <= 1.0f);
        aiVector3D start = pNodeAnim->mPositionKeys[positionIndex].mValue;
        aiVector3D end = pNodeAnim->mPositionKeys[nextPositionIndex].mValue;
        aiVector3D delta = end - start;

        return start + factor * delta; 
    }

    // To animator/animation
    aiQuaternion CalcInterpolatedRotation(float pAnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumRotationKeys == 1)
        {
            return pNodeAnim->mRotationKeys[0].mValue;
        }

        unsigned int rotationIndex = FindRotation(pAnimationTime, pNodeAnim);
        unsigned int nextRotationIndex = rotationIndex + 1;
        assert(nextRotationIndex < pNodeAnim->mNumRotationKeys);

        float deltaTime = (float)(pNodeAnim->mRotationKeys[nextRotationIndex].mTime - pNodeAnim->mRotationKeys[rotationIndex].mTime);
        float factor = (pAnimationTime - (float)pNodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
        assert(factor >= 0.0f && factor <= 1.0f);

        aiQuaternion startQuat = pNodeAnim->mRotationKeys[rotationIndex].mValue;
        aiQuaternion endQuat = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;

        return nlerp(startQuat, endQuat, factor); 
    }

    // To animator/animation
    aiVector3D CalcInterpolatedScaling(float pAnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumScalingKeys == 1)
        {
            return pNodeAnim->mScalingKeys[0].mValue;
        }

        unsigned int scalingIndex = FindScaling(pAnimationTime, pNodeAnim);
        unsigned int nextScalingIndex = scalingIndex + 1;
        assert(nextScalingIndex < pNodeAnim->mNumScalingKeys);

        float deltaTime = (float)(pNodeAnim->mScalingKeys[nextScalingIndex].mTime - pNodeAnim->mScalingKeys[scalingIndex].mTime);
        float factor = (pAnimationTime - (float)pNodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;
        assert(factor >= 0.0f && factor <= 1.0f);
        aiVector3D start = pNodeAnim->mScalingKeys[scalingIndex].mValue;
        aiVector3D end = pNodeAnim->mScalingKeys[nextScalingIndex].mValue;
        aiVector3D delta = end - start;

        return start + factor * delta; 
    }

    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string pNodeName)
    {
        for (unsigned int i = 0; i < pAnimation->mNumChannels; i++)
        {
            const aiNodeAnim* nodeAnim = pAnimation->mChannels[i];
            if (string(nodeAnim->mNodeName.data) == pNodeName)
            {
                return nodeAnim;
            }
        }

        return nullptr;
    }

    void BoneTransform(double timeInSeconds, vector<aiMatrix4x4>& transforms, Animation animation)
    {
        aiMatrix4x4 identityMatrix;

        double timeInTicks = timeInSeconds * animation.ticksPerSecond;
        this->currentAnimationTime += timeInTicks;
        CheckIfEnded();
        this->currentAnimationTime = fmod(currentAnimationTime, animation.scene->mAnimations[0]->mDuration);

        ReadNodeHierarchy(currentAnimationTime, animation.scene->mRootNode, identityMatrix, animation.scene);

        transforms.resize(animatedModel->mNumBones);

        for (unsigned int i = 0; i < animatedModel->mNumBones; i++)
        {
            transforms[i] = animatedModel->mBoneMatrices[i].final_world_transform;
        }

    }

    void ReadNodeHierarchy(float pAnimationTime, const aiNode* pNode, const aiMatrix4x4 parentTransform, const aiScene* scene)
    {
        string nodeName(pNode->mName.data);

        const aiAnimation* animation = scene->mAnimations[0];
        aiMatrix4x4 nodeTransform = pNode->mTransformation;

        const aiNodeAnim* nodeAnim = FindNodeAnim(animation, nodeName);

        if (nodeAnim)
        {
            // Scaling
            aiVector3D scalingVector = CalcInterpolatedScaling(pAnimationTime, nodeAnim);
            aiMatrix4x4 scalingMat;
            aiMatrix4x4::Scaling(scalingVector, scalingMat);

            // Rotation
            aiQuaternion rotateQuat = CalcInterpolatedRotation(pAnimationTime, nodeAnim);
            aiMatrix4x4 rotationMat = aiMatrix4x4(rotateQuat.GetMatrix());

            // Translation
            aiVector3D translateVector = CalcInterpolatedPosition(pAnimationTime, nodeAnim);
            aiMatrix4x4 translationMat;
            aiMatrix4x4::Translation(translateVector, translationMat);

            nodeTransform = translationMat * rotationMat * scalingMat;
        }

        aiMatrix4x4 globalTransform = parentTransform * nodeTransform;

        // Change to model->mBoneMapping
        if (animatedModel->mBoneMapping.find(nodeName) != animatedModel->mBoneMapping.end())
        {
            // Change to model->mBoneMapping
            unsigned int boneIndex = animatedModel->mBoneMapping[nodeName];
            // Change to model->mBoneMatrices
            animatedModel->mBoneMatrices[boneIndex].final_world_transform = animatedModel->mGlobalInverseTransform * globalTransform * animatedModel->mBoneMatrices[boneIndex].offset_matrix;
        }

        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            ReadNodeHierarchy(pAnimationTime, pNode->mChildren[i], globalTransform, scene);
        }
    }
    
    aiQuaternion nlerp(aiQuaternion a, aiQuaternion b, float blend)
    {
        //cout << a.w + a.x + a.y + a.z << endl;
        a.Normalize();
        b.Normalize();

        aiQuaternion result;
        float dot_product = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        float one_minus_blend = 1.0f - blend;

        if (dot_product < 0.0f)
        {
            result.x = a.x * one_minus_blend + blend * -b.x;
            result.y = a.y * one_minus_blend + blend * -b.y;
            result.z = a.z * one_minus_blend + blend * -b.z;
            result.w = a.w * one_minus_blend + blend * -b.w;
        }
        else
        {
            result.x = a.x * one_minus_blend + blend * b.x;
            result.y = a.y * one_minus_blend + blend * b.y;
            result.z = a.z * one_minus_blend + blend * b.z;
            result.w = a.w * one_minus_blend + blend * b.w;
        }

        return result.Normalize();
    }
    


};

#endif