#ifndef WATER_H
#define WATER_H

#include "extModel.h"

class Water : public ExtModel
{
public:
    Water();

private:
    void GeneratePointCloud();

};

#endif // WATER_H
