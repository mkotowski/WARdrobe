#ifndef MODEL_H
#define MODEL_H
#define MAX_BONES_IN_SHADER 16

/**
 * Więc tak...
 * Tutorial(e) 1 -  https://realitymultiplied.wordpress.com/2016/05/02/assimp-skeletal-animation-tutorial-1-vertex-weights-and-indices/
 * Jest to drugi tutorial, z którego korzystałem, on sam bazuje na innym, pierwotnym tutorialu:
 * http://www.ogldev.org/www/tutorial38/tutorial38.html
 * 
 * Tutorial 2 - polecony przez Gałaja, implementacja w Javie:
 * https://www.youtube.com/watch?v=f3Cr8Yx3GGA - YT
 * https://github.com/TheThinMatrix/OpenGL-Animation - GitHub
 */
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
#include <Bone.hpp>
#include <Animation.hpp>


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <utility>
using namespace std;

class Model 
{
public:
    /*  Model Data */
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    vector<Animation> animations;
    string directory;
    unsigned int textureID;
    bool gammaCorrection;
    Bone* rootBone;
    glm::mat4 globalInverseTransform;
    int boneCount;
    glm::mat4 boneMats[MAX_BONES_IN_SHADER];


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
    void Draw(unsigned int shaderID, float dt)
    {
        this->GetBoneTransforms();
        glUseProgram(shaderID);
        
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "gBones"), 
                                                  MAX_BONES_IN_SHADER, 
                                                  GL_FALSE, 
                                                  glm::value_ptr(this->boneMats[0]));
        
        for(unsigned int i = 0; i < meshes.size(); i++)
        {
            meshes[i].Draw(shaderID);
        }                
    }

    // Aktualizuje tablicę boneMats/gBones w vertexShader
    void GetBoneTransforms()
    {
        for (Bone* bone : bones)
        {
            CalculateBoneMat(bone);
        }        
    }

private:
    std::vector<aiNode*> aiNodes;
    const aiScene* scene;
    std::vector<Bone*> bones;
    vector<int> boneID_s;
    vector<float> boneWeights;

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
        std::cout << "Loads model" << std::endl;
        // read file via ASSIMP
        Assimp::Importer importer;
        this->scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }

        // process ASSIMP's root node recursively
        std::cout << scene->mRootNode->mName.data << std::endl;
        LoadAllNodes(scene->mRootNode);
        LoadBones(scene);
        processNode(scene->mRootNode, scene, texturePath);     
          // najpierw kości, później meshe 

        if (scene->mNumAnimations > 0)
        {
            LoadAnimations(scene);        
        }
            
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene, string texturePath)
    {
        std::cout << node->mName.data << " node name" << std::endl;
        
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
            vertex.BoneIDs[0] = -1;
            vertex.BoneIDs[1] = -1;
            vertex.BoneIDs[2] = -1;
            vertex.BoneIDs[3] = -1;

            vertex.Weights[0] = 0.0;
            vertex.Weights[1] = 0.0;
            vertex.Weights[2] = 0.0;
            vertex.Weights[3] = 0.0;

            // tutaj
            // jeżeli mesh ma kość -> znajdź kość i weź wagę oraz ID dla danego wierzchołka 
            int weightID = 0;
            for (int j = 0; j < mesh->mNumBones; j++)
            {
                Bone* tempBone = FindBoneByName(mesh->mBones[j]->mName.data);
                if (tempBone == nullptr)
                {
                    std::cout << "NULL" << std::endl;
                    getchar();
                }
                for (int k = 0; k < mesh->mBones[j]->mNumWeights; k++)
                {
                    if (mesh->mBones[j]->mWeights[k].mVertexId == i)
                    {
                        vertex.BoneIDs[weightID] = tempBone->ID;
                        vertex.Weights[weightID] =  mesh->mBones[j]->mWeights[k].mWeight;
                        weightID++;
                        break;
                    }
                }       
                if (weightID >= 4)
                    break;             
            }

            for (int j = 1; j < 4; j++)
            {
                if (vertex.BoneIDs[0] == -1)
                {
                    std::cout << "NO BONE" << std::endl;
                }
                if (vertex.BoneIDs[j] == -1)
                {
                    vertex.BoneIDs[j] = 0;
                    vertex.Weights[j] = 0.01;
                }                    
            }

            vertices.push_back(vertex);
        }

        std::cout << vertices.size() << std::endl;
        
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
        // return a mesh object created from the extracted mesh data      
    
        return Mesh(vertices, indices, textures);
    }

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName)
    {
        /*
        vector<Texture> textures;

        Texture texture;
        texture.id = TextureFromFile()
        */
        /*
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            bool skip = false;
            for(unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if(std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                
                Texture texture;
                texture.id = TextureFromFile(str.C_Str());
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
        */
        //return textures;
    }

    // Dodaje macierze do boneMats/gBones
    void AddBonesToArray(Bone* bone)
    {
        this->boneMats[bone->ID] = bone->GetAnimatedTransform();
        for (Bone* boneCH : bone->children)
        {
            AddBonesToArray(boneCH);
        }
    }

    // Ładuje kości
    void LoadBones(const aiScene* scene)
    {
        for (int i = 0; i < scene->mNumMeshes; i++)
        {
            for (int j = 0; j < scene->mMeshes[i]->mNumBones; j++)
            {
                std::string boneName = scene->mMeshes[i]->mBones[j]->mName.data;
                glm::mat4 mat = AiToGLMMat4((FindAiNode(boneName)->mTransformation));
                glm::mat4 offsetMatrix = AiToGLMMat4(scene->mMeshes[i]->mBones[j]->mOffsetMatrix);
                int index = bones.size();
                bones.push_back(new Bone(index, boneName, mat, offsetMatrix));
            }
        }        
        SetHierarchy(scene);
        rootBone->CalcInverseBindTransform(glm::mat4(1.0f));
    }

    // Zwraca wskaźnik do aiNode w wektorze po nazwie
    aiNode* FindAiNode(std::string nodeName)
    {
        for (int i = 0; i < aiNodes.size(); i++)
        {
            if (aiNodes.at(i)->mName.data == nodeName)
                return aiNodes.at(i);
        }
        return nullptr;
    }

    // Zwraca wskaźnik do Kości w wektorze po nazwie
    Bone* FindBoneByName(std::string name)
    {
        for (Bone* bone : bones)
        {
            if (bone->name == name)
                return bone;
        }
        return nullptr;
    }

    // Tworzy hierachię (szkielet)
    void SetHierarchy(const aiScene* scene)
    {
        this->globalInverseTransform = glm::inverse(AiToGLMMat4(scene->mRootNode->mTransformation));
        for (Bone* bone : bones)
        {
            // Sets rootBone
            if (FindBoneByName(FindAiNode(bone->name)->mParent->mName.data) == nullptr)
            {
                this->rootBone = bone;
                bone->parentBone = "NO_PARENT";
            }
                
            // Sets children
            aiNode* tempNode = FindAiNode(bone->name);
            for (int i = 0; i < tempNode->mNumChildren; i++)
            {
                bone->AddChild(FindBoneByName(tempNode->mChildren[i]->mName.data));
                bone->children.at(i)->parentBone = bone->name;
            }               
        }
        PrintHierarchy(this->rootBone);
    }

    // Wyświetla hierarchię kości
    void PrintHierarchy(Bone* bone)
    {
        std::cout << bone->ID << " | " << bone->name << " | " << bone->children.size() << " | " <<  bone->parentBone << std::endl;
        
        for (auto x : bone->children)
        {
            PrintHierarchy(x);
        }        
        
    }

    // Ładowanie animacji 
    void LoadAnimations(const aiScene* scene)
    {
        for (int i = 0; i < scene->mNumAnimations; i++)
        {
            Animation animation = Animation();
            animation.name = scene->mAnimations[i]->mName.data;
            animation.length = scene->mAnimations[i]->mDuration;
            std::cout << scene->mAnimations[i]->mNumChannels << std::endl;
            // cheking number of keyframes
            for (int j = 0; j < scene->mAnimations[i]->mChannels[0]->mNumPositionKeys; j++)
            {
                KeyFrame keyframe = KeyFrame();
                // timeStamp zmienić na double'a
                keyframe.timeStamp = scene->mAnimations[i]->mChannels[0]->mPositionKeys[j].mTime;
                for (int k = 0; k < scene->mAnimations[i]->mNumChannels; k++)
                {
                    BoneTransform boneTransform = BoneTransform();
                    keyframe.pose.insert(std::pair<std::string, BoneTransform>(scene->mAnimations[i]->mChannels[k]->mNodeName.data, boneTransform));
                }


                animation.keyFrames.push_back(keyframe);              
            }

            // może jedna pętla jednak?
            // a gdzie skala?
            for (int k = 0; k < scene->mAnimations[i]->mNumChannels; k++)
            {
                for (int z = 0; z < scene->mAnimations[i]->mChannels[0]->mNumPositionKeys; z++)
                {
                    animation.keyFrames.at(z).pose.find(scene->mAnimations[i]->mChannels[k]->mNodeName.data)->second.position =
                                                        glm::vec3(scene->mAnimations[i]->mChannels[k]->mPositionKeys[z].mValue.x,
                                                                  scene->mAnimations[i]->mChannels[k]->mPositionKeys[z].mValue.y,
                                                                  scene->mAnimations[i]->mChannels[k]->mPositionKeys[z].mValue.z);

                    glm::quat quat = glm::quat(1.0f, 1.0f, 1.0f, 1.0f);
                    quat.w = scene->mAnimations[i]->mChannels[k]->mRotationKeys[z].mValue.w;
                    quat.x = scene->mAnimations[i]->mChannels[k]->mRotationKeys[z].mValue.x;
                    quat.y = scene->mAnimations[i]->mChannels[k]->mRotationKeys[z].mValue.y;
                    quat.z = scene->mAnimations[i]->mChannels[k]->mRotationKeys[z].mValue.z;
                    animation.keyFrames.at(z).pose.find(scene->mAnimations[i]->mChannels[k]->mNodeName.data)->second.rotation = quat;
                }
            }
            this->animations.push_back(animation);
        }
    }

    // Załądowanie wszystkich aiNode'ów do wektorów
    void LoadAllNodes(aiNode* node)
    {
        aiNodes.push_back(node);
        for (int i = 0; i < node->mNumChildren; i++)
        {
            LoadAllNodes(node->mChildren[i]);
        }
    }

    // Oblicza boneMats według tutoriala 1 - coś z tym jest zepsute
    void CalculateBoneMat(Bone* bone)
    {
        if (bone->parentBone == "NO_PARENT")
        {
            this->boneMats[bone->ID] = globalInverseTransform * bone->localBindTransform * bone->offsetMatrix;
        }
        else
        {
            this->boneMats[bone->ID] = globalInverseTransform * CalculateParentMat(bone) * bone->localBindTransform * bone->offsetMatrix;
        }        
    }

    // Oblicza transform parentBone'a - tutorial 1
    glm::mat4 CalculateParentMat(Bone* bone)
    {
        std::string parentBoneName = bone->parentBone;
        std::vector<glm::mat4> mats;
        while (parentBoneName != "NO_PARENT")
        {
            mats.push_back(FindBoneByName(parentBoneName)->localBindTransform);
            parentBoneName = FindBoneByName(parentBoneName)->parentBone;
        }
        glm::mat4 concatenatedTransforms = glm::mat4(1.0f);
        for (int i = mats.size()-1; i >= 0; i--)
            concatenatedTransforms *= mats.at(i);

        return concatenatedTransforms;
        
    }
};



#endif