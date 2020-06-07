#ifndef MODELARRAY_H
#define MODELARRAY_H
#include "Model.hpp"

class ModelArray
{
public:
    // Bool determining if LoD should be used: false - no, true - yes
    int checkLevelOfDetail;
    // zeroLevelModel   - renders when LoD is off
    Model zeroLevelModel;
    // firstLevelModel  - renders when model is between camera and first level distance
    Model firstLevelModel;
    // secondLevelModel - renders when model is between first level distance and second level distance
    Model secondLevelModel;
    // thirdLevelModel  - renders when model is between second level distance and cutoff distance
    Model thirdLevelModel;
    ModelArray()
    {
        checkLevelOfDetail = 0;
    }
    ModelArray(int checkLevelOfDetail)
    {
        this->checkLevelOfDetail = checkLevelOfDetail;
    }

};

#endif