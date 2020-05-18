#include "Bone.hpp"

Bone::Bone()
{
    this->ID = -2;
    this->name = "";
}

/**
 * Konstruktor  - indeks (kolejność ładowania), nazwa, bindLocalTransform (mTransformation w assimpie), 
 * ofsetMatrix (wykorzystywany w implementacji C++, tutoriale 1) 
 */
Bone::Bone(int index, std::string name, glm::mat4 bindLocalTransform, glm::mat4 offsetMatrix)
{
    this->ID = index;
    this->name = name;
    this->localBindTransform = bindLocalTransform;
    this->offsetMatrix = offsetMatrix;
}

/**
 * Dodaje wskaźnik do kości do wektora dzieci
 */
void Bone::AddChild(Bone* bone)
{
    this->children.push_back(bone);
}

/**
 * Zwraca animowanyTransform, wykorzystywany w implenetacji z Javy
 */
glm::mat4 Bone::GetAnimatedTransform()
{
    return this->animatedTransform;
}

/**
 * Zwraca inverseBindTransform 
 */
glm::mat4 Bone::GetInverseBindTransform()
{
    return this->inverseBindTransform;
}
/**
 * Dla każdego z dzieci oblicza InverseBindTransform umożliwiający uzyskanie transforma względem pozycji modelu (model-space)
 * a nie samej kości, według tutoriala nr 2 (implementacja w javie)
 */
void Bone::CalcInverseBindTransform(glm::mat4 parentBindTransform)
{
    glm::mat4 bindTransform = parentBindTransform * this->localBindTransform;
    PrintMat4(parentBindTransform);
    std::cout << "----------------------------------------" << std::endl;
    this->inverseBindTransform = glm::inverse(bindTransform);
    for (Bone* child : children)
    {
        child->CalcInverseBindTransform(bindTransform);
    }
    
}
/**
 * Ustawia animatedTransform, wykorzystywane przy implementacji z Javy
 */
void Bone::SetAnimationTransform(glm::mat4 animationTransform)
{
    this->animatedTransform = animationTransform;
}

