#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "LoadTGA.h"
#include "LittleOBJLoader.h"
#include "GL_utilities.h"
#include "pointCloud.h"

#include <map>
#include <string>

class AssetManager
{
public:
    AssetManager(std::string const&);
    
    GLuint getTexture(std::string const&) const;
    GLuint getShader(std::string const&) const;
    Model* getModel(std::string const&) const;
    PointCloud& getPointCloud(std::string const&);

    void loadAssets(std::string const&);

private:
    std::map<std::string, GLuint> textures;
    std::map<std::string, GLuint> shaders;
    std::map<std::string, Model*> models;
    std::map<std::string, PointCloud> pointClouds;
};

struct Drawable
{
    Model* model;
    GLuint shader;
};

#endif //ASSETMANAGER_H
