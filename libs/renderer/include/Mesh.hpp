#ifndef MESH_H
#define MESH_H

#define NUM_BONES_PER_VERTEX 4

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>

using namespace std;

void ZeroMat(aiMatrix4x4 *);

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct BoneInfo
{
    aiMatrix4x4 BoneOffset;
    aiMatrix4x4 FinalTransformation;

    BoneInfo()
    {
        ZeroMat(&BoneOffset);
        ZeroMat(&FinalTransformation);
    }

};

struct VertexBoneData
{
    unsigned int IDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX];
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh 
{
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    unsigned int numBones;
    map<string, unsigned int> boneMapping;
    VertexBoneData bones;
    vector<BoneInfo> boneInfo;
    aiMatrix4x4 globalInverseTransform;
    unsigned int VAO;

    /*  Functions  */
    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, VertexBoneData bones, vector<BoneInfo> boneInfo, map<string, unsigned int> boneMapping, unsigned int numBones)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->bones = bones;
        this->boneInfo = boneInfo;
        this->boneMapping = boneMapping;
        this->numBones = numBones;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(unsigned int shaderID) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shaderID, "texture1"), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    /*  Render data  */
    unsigned int VBO, EBO;

    /*  Functions    */
    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        // bone ids
        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(5, 4, GL_INT, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, IDs));
        // bone weights
        glEnableVertexAttribArray(6);
        glVertexAttribIPointer(6, 4, GL_INT, sizeof(VertexBoneData), (void*)offsetof(VertexBoneData, Weights));


        glBindVertexArray(0);
    }
};

void ZeroMat(aiMatrix4x4 * mat)
{
    mat->a1 = 0.0f; mat->a2 = 0.0f; mat->a3 = 0.0f; mat->a4 = 0.0f;
    mat->b1 = 0.0f; mat->b2 = 0.0f; mat->b3 = 0.0f; mat->b4 = 0.0f;
    mat->c1 = 0.0f; mat->c2 = 0.0f; mat->c3 = 0.0f; mat->c4 = 0.0f;
    mat->d1 = 0.0f; mat->d2 = 0.0f; mat->d3 = 0.0f; mat->d4 = 0.0f;    
};

void InitMat(aiMatrix4x4 * mat)
{
    mat->a1 = 1.0f; mat->a2 = 0.0f; mat->a3 = 0.0f; mat->a4 = 0.0f;
    mat->b1 = 0.0f; mat->b2 = 1.0f; mat->b3 = 0.0f; mat->b4 = 0.0f;
    mat->c1 = 0.0f; mat->c2 = 0.0f; mat->c3 = 1.0f; mat->c4 = 0.0f;
    mat->d1 = 0.0f; mat->d2 = 0.0f; mat->d3 = 0.0f; mat->d4 = 1.0f;
};

void InitScaleTransform(aiMatrix4x4 * mat, float scaleX, float scaleY, float scaleZ)
{
    mat->a1 = scaleX;   mat->a2 = 0.0f;     mat->a3 = 0.0f;     mat->a4 = 0.0f;
    mat->b1 = 0.0f;     mat->b2 =scaleX;    mat->b3 = 0.0f;     mat->b4 = 0.0f;
    mat->c1 = 0.0f;     mat->c2 = 0.0f;     mat->c3 = scaleZ;   mat->c4 = 0.0f;
    mat->d1 = 0.0f;     mat->d2 = 0.0f;     mat->d3 = 0.0f;     mat->d4 = 1.0f;
};

void InitTranslationTransform(aiMatrix4x4 * mat, float x, float y, float z)
{
    mat->a1 = 1.0f; mat->a2 = 0.0f; mat->a3 = 0.0f; mat->a4 = x;
    mat->b1 = 0.0f; mat->b2 = 1.0f; mat->b3 = 0.0f; mat->b4 = y;
    mat->c1 = 0.0f; mat->c2 = 0.0f; mat->c3 = 1.0f; mat->c4 = z;
    mat->d1 = 0.0f; mat->d2 = 0.0f; mat->d3 = 0.0f; mat->d4 = 1.0f;
};

void Mat3x3To4x4(aiMatrix4x4 * mat, aiMatrix3x3 mat2)
{
    mat->a1 = mat2.a1; mat->a2 =mat2.a2; mat->a3 = mat2.a3; mat->a4 = 0.0f;
    mat->b1 = mat2.b1; mat->b2 =mat2.b2; mat->b3 = mat2.b3; mat->b4 = 0.0f;
    mat->c1 = mat2.c1; mat->c2 =mat2.c2; mat->c3 = mat2.c3; mat->c4 = 0.0f;
    mat->d1 = 0.0f; mat->d2 = 0.0f; mat->d3 = 0.0f; mat->d4 =1.0f;
};
#endif