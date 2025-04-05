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
    ExtModel(ExtModel const&) = delete;
    ExtModel(ExtModel &&) = delete;
    ~ExtModel();

    ExtModel& operator=(ExtModel const&) = delete;
    ExtModel& operator=(ExtModel &&) = delete;

    void draw(mat4 const&, mat4 const&, mat4 const&, GLuint);
    void setShader(GLuint&);
    GLuint& getShader();

private:
    vec3 position;
    std::vector<int> texIDs;

protected:
    GLuint* shader;
    Model* model;
};

#endif //EXTMODEL_H
