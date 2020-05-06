#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh.hpp>
#include <Shader.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <assert.h>
using namespace std;



class Model 
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    unsigned int textureID;
    bool gammaCorrection;

    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model()
    {
        
    }
    Model(string const &modelPath, string const &texturesPath, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(modelPath, texturesPath);
        //textureID = TextureFromFile(texturesPath.c_str());
        
    }

    // draws the model, and thus all its meshes
    void Draw(unsigned int shaderID)
    {
        
        for(unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shaderID);
    }
    
private:
    unsigned int TextureFromFile(string path)
    {
        string filename = path;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
}

    /*  Functions   */
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path, string texturePath)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        

        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene, texturePath);
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene, string texturePath)
    {
        // process each mesh located at the current node
        for(unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            // the node object only contains indices to index the actual objects in the scene. 
            // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene, texturePath));
        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene, texturePath);
        }

    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene, string texturePath)
    {
        // data to fill
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        
        unsigned int numBones;
        map<string, unsigned int> boneMapping;
        VertexBoneData bones;
        vector<BoneInfo> boneInfo;
        aiMatrix4x4 globalInverseTransform;

        // Walk through each of the mesh's vertices
        for(unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            // positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;
            vertex.Position = vector;
            // normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
            // texture coordinates
            if(mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x; 
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
            vertices.push_back(vertex);
        }
        // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            // retrieve all indices of the face and store them in the indices vector
            for(unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        // process materials
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];    
        // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
        // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
        // Same applies to other texture as the following list summarizes:
        // diffuse: texture_diffuseN
        // specular: texture_specularN
        // normal: texture_normalN

        // 1. diffuse maps

        Texture texture;

        texture.id = TextureFromFile(texturePath);
        texture.type = aiTextureType_DIFFUSE;
        texture.path = texturePath;
        textures.push_back(texture);
        textures_loaded.push_back(texture);

        /*
        vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        */

        // Process animation

        if(mesh->HasBones())
        {
            LoadBones(vertices, mesh, bones, boneInfo, boneMapping, numBones);
        }
        

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures, bones, boneInfo, boneMapping, numBones);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        
    }

    void LoadBones(vector<Vertex> &vertices, aiMesh *mesh, VertexBoneData &bones, vector<BoneInfo> &boneInfo, map<string, unsigned int> &boneMapping, unsigned int &numBones)
    {
        
        for (unsigned int i = 0; i < mesh->mNumBones; i++)
        {
            unsigned int BoneIndex = 0;
            string BoneName(mesh->mBones[i]->mName.data);

            if (boneMapping.find(BoneName) == boneMapping.end())
            {
                BoneIndex = numBones;
                numBones++;
                BoneInfo bi;
                boneInfo.push_back(bi);
            }
            else
            {
                BoneIndex = boneMapping[BoneName];
            }
            
            boneMapping[BoneName] = BoneIndex;
            boneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;

            for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
            {
                unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
                float Weight = mesh->mBones[i]->mWeights[j].mWeight;
                
                for (unsigned int k = 0 ; k < NUM_BONES_PER_VERTEX; k++) 
                {
                    if (bones.Weights[k] == 0.0f) 
                    {
                        bones.IDs[k] = BoneIndex;
                        bones.Weights[k] = Weight;
                        break;
                    }        
                }
            }


        }
    }

    aiMatrix4x4 BoneTransform(float TimeInSeconds, vector<aiMatrix4x4> &Transforms, aiScene *scene, unsigned int numBones, vector<BoneInfo> *boneInfo)
    {
        aiMatrix4x4 identity;
        InitMat(&identity);

        float TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;
        float TimeInTicks = TimeInSeconds * TicksPerSecond;
        float AnimationTIme = fmod(TimeInTicks, scene->mAnimations[0]->mDuration);

        ReadNodeHierarchy

        Transforms.resize(numBones);

        for (unsigned int i = 0; i < numBones; i++)
        {
            Transforms[i] = boneInfo->at(i).FinalTransformation;
        }

    }

    const aiNodeAnim* FindNodeAnim(const aiAnimation *pAnimation, const string nodeName)
    {
        for (unsigned int i = 0; i < pAnimation->mNumChannels; i++)
        {
            const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

            if (string(pNodeAnim->mNodeName.data) == nodeName)
            {
                return pNodeAnim;
            }

            return NULL;
        }
    }

    unsigned int FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim)
    {
        assert(pNodeAnim->mNumRotationKeys > 0);

        for (unsigned int i= 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
        {
            if (AnimationTime < (float)pNodeAnim->mRotationKeys[i+1].mTime)
            {
                return 1;
            }
        }

        assert(0);
    }

    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        assert(pNodeAnim->mNumScalingKeys > 0);
        
        for (unsigned int i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) 
        {
            if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) 
            {
                return i;
            }
        }
    
        assert(0);

        return 0;
    }

    unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
    {    
        for (unsigned int i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) 
        {
            if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) 
            {
                return i;
            }   
        }
    
        assert(0);

        return 0;
    }

    void CalcInterpolatedScaling(aiVector3D& out, float AnimationTime, const aiNodeAnim *pNodeAnim)
    {
        if (pNodeAnim->mNumScalingKeys == 1) 
        {
            out = pNodeAnim->mScalingKeys[0].mValue;
            return;
        }

        unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
        unsigned int NextScalingIndex = (ScalingIndex + 1);
        assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
        float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
        const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
        aiVector3D Delta = End - Start;
        out = Start + Factor * Delta;
    }


    void CalcInterpolatedRotation(aiQuaternion& out, float AnimationTime, const aiNodeAnim *pNodeAnim)
    {
        if (pNodeAnim->mNumRotationKeys == 1)
        {
            out = pNodeAnim->mRotationKeys[0].mValue;
            return;
        }

        unsigned int rotationIndex = FindRotation(AnimationTime, pNodeAnim);
        unsigned int nextRotationIndex = (rotationIndex + 1);
        assert(nextRotationIndex < pNodeAnim->mNumRotationKeys);
        float deltaTime = pNodeAnim->mRotationKeys[nextRotationIndex].mTime - pNodeAnim->mRotationKeys[rotationIndex].mTime;
        float factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;
        assert(factor >= 0.0f && factor <= 1.0f);
        const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[rotationIndex].mValue;
        const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[nextRotationIndex].mValue;
        aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, factor);
        out = out.Normalize();
    }

    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
    {
        if (pNodeAnim->mNumPositionKeys == 1) {
            Out = pNodeAnim->mPositionKeys[0].mValue;
            return;
        }
                
        unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
        unsigned int NextPositionIndex = (PositionIndex + 1);
        assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
        float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
        float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
        assert(Factor >= 0.0f && Factor <= 1.0f);
        const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
        const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
        aiVector3D Delta = End - Start;
        Out = Start + Factor * Delta;
}


    void ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 ParentTransform, aiScene *scene, map<string, unsigned int> *boneMapping, vector<BoneInfo> *boneInfo, aiMatrix4x4 globalInverseTransform)
    {
        string nodeName(pNode->mName.data);

        const aiAnimation *pAnimation = scene->mAnimations[0];

        aiMatrix4x4 NodeTransformation(pNode->mTransformation);

        const aiNodeAnim *pNodeAnim = FindNodeAnim(pAnimation, nodeName);

        if (pNodeAnim)
        {
            // Interpolate scaling and generate scaling transformation matrix
            aiVector3D scaling;
            CalcInterpolatedScaling(scaling, AnimationTime, pNodeAnim);
            aiMatrix4x4 scalingM;
            InitScaleTransform(&scalingM, scaling.x, scaling.y, scaling.z);

            aiQuaternion rotationQ;
            CalcInterpolatedRotation(rotationQ, AnimationTime, pNodeAnim);
            aiMatrix4x4 rotationM;
            Mat3x3To4x4(&rotationM, rotationQ.GetMatrix());

            aiVector3D translation;
            CalcInterpolatedPosition(translation, AnimationTime, pNodeAnim);
            aiMatrix4x4 translationM;
            InitTranslationTransform(&translationM, translation.x, translation.y, translation.z);
        
            NodeTransformation = translationM * rotationM * scalingM;
        }

        aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

        if (boneMapping->find(nodeName) != boneMapping->end())
        {
            unsigned int BoneIndex = boneMapping->at(nodeName);
            boneInfo->at(BoneIndex).FinalTransformation = globalInverseTransform * GlobalTransformation * boneInfo->at(BoneIndex).BoneOffset;
        }

        for (unsigned int i = 0; i < pNode->mNumChildren; i++)
        {
            ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, scene, boneMapping, boneInfo, globalInverseTransform);
        }
    }    

    void RunAnimation(float runningTime)
    {
        vector<aiMatrix4x4> transforms;
        for (auto& mesh : meshes)
        {
            this->BoneTransform(runningTime, transforms, , mesh.numBones, &mesh.boneInfo);
        }
    }
};





#endif