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

#define MAX_BONES_IN_SHADER 32

using namespace std;



class Model 
{
public:
    /*  Model Data */
    map<string, unsigned int> mBoneMapping;
    vector<BoneMatrix> mBoneMatrices;
    aiMatrix4x4 mGlobalInverseTransform;
    vector<aiMatrix4x4> transforms;
    unsigned int mNumBones = 0;
    GLuint mBoneLocation[MAX_BONES_IN_SHADER];
    /*  Functions   */
    // constructor, expects a filepath to a 3D model.
    Model()
    {
        
    }
    Model(string const &modelPath, string const &texturesPath,bool gamma = false) : gammaCorrection(gamma)
    {
        
        loadModel(modelPath, texturesPath);
        
        //textureID = TextureFromFile(texturesPath.c_str());
        
    }

    // draws the model, and thus all its meshes
    void Draw(unsigned int shaderID, float dt)
    {
        
        InitShaders(shaderID);

        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shaderID);
    }
    
private:
    vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh> meshes;
    string directory;
    unsigned int textureID;
    bool gammaCorrection;
    const aiScene* scene;
    float currentAnimationTime = 0;



    float ticksPerSecond = 0.0f;

    void InitShaders(GLuint shaderID)
    {
        for (unsigned int i = 0; i < MAX_BONES_IN_SHADER; i++)
        {
            string name = "gBones[" + to_string(i) + "]";
            mBoneLocation[i] = glGetUniformLocation(shaderID, name.c_str());
        }
    }

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
        importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        scene = importer.GetOrphanedScene();
        // check for errors
        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        
        mGlobalInverseTransform = scene->mRootNode->mTransformation;
        mGlobalInverseTransform.Inverse();

        if (scene->mAnimations[0]->mTicksPerSecond != 0.0)
        {
            ticksPerSecond = scene->mAnimations[0]->mTicksPerSecond;
        }
        else
        {
            ticksPerSecond = 25.0f;
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
        vector<VertexBoneData> boneData;

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

        Texture texture;

        texture.id = TextureFromFile(texturePath);
        texture.type = aiTextureType_DIFFUSE;
        texture.path = texturePath;
        textures.push_back(texture);
        textures_loaded.push_back(texture);

        boneData.resize(mesh->mNumVertices);

        for (unsigned int i = 0; i < mesh->mNumBones; i++)
        {
                unsigned int boneIndex = 0;
                string boneName(mesh->mBones[i]->mName.data);

                if (mBoneMapping.find(boneName) == mBoneMapping.end())
                {
                    
                    boneIndex = mNumBones;
                    mNumBones++;
                    BoneMatrix boneMat;
                    mBoneMatrices.push_back(boneMat);
                    mBoneMatrices[boneIndex].offset_matrix = mesh->mBones[i]->mOffsetMatrix;
                    mBoneMapping[boneName] = boneIndex;
                }
                else
                {
                    boneIndex = mBoneMapping[boneName];
                }
                    
                for (unsigned int j = 0; j < mesh->mBones[i]->mNumWeights; j++)
                {
                    unsigned int vertexID = mesh->mBones[i]->mWeights[j].mVertexId;
                    float weight = mesh->mBones[i]->mWeights[j].mWeight;
                
                    boneData[vertexID].addBoneData(boneIndex, weight);
                
                }                
        }        

        // return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures, boneData);
    }
    
};



#endif