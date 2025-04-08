#ifndef EXTMODEL_H
#define EXTMODEL_H

#include "VectorUtils4.h"
#include "LittleOBJLoader.h"
#include "assetManager.h"

#include <vector>

class ExtModel
{
public:
    ExtModel(AssetManager const&);
    ExtModel(Model*, GLuint const);
    ExtModel(ExtModel const&) = default;
    ExtModel(ExtModel &&) = default;
    ~ExtModel() = default;

    ExtModel& operator=(ExtModel const&) = default;
    ExtModel& operator=(ExtModel &&) = default;

    void draw(mat4 const&, mat4 const&) const;
    void setShader(GLuint const);
    GLuint getShader() const;

    mat4 getModelToWorld() const;
    Model* getModel() const;

    void setPosition();
private:
    vec3 position;
    std::vector<int> texIDs;

protected:
    Model* model;
    GLuint shader;
};

#endif //EXTMODEL_H
