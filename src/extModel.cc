#include "extModel.h"

ExtModel::ExtModel()
: position{}, model{}, texIDs{}
{}

ExtModel::ExtModel(Model* model)
: position{}, model{model}, texIDs{}
{}

ExtModel::~ExtModel()
{
    delete model;
}
