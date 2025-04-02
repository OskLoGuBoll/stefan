#ifndef EXTMODEL_H
#define EXTMODEL_H

#include "VectorUtils4.h"
#include "LittleOBJLoader.h"

#include <vector>

class ExtModel
{
public:
    ExtModel();
    ExtModel(Model*);
    ~ExtModel();

private:
    vec3 position;
    Model* model;
    std::vector<int> texIDs;
};

#endif //EXTMODEL_H
