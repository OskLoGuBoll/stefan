#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "LoadTGA.h"
#include "LittleOBJLoader.h"
#include "GL_utilities.h"

#include <map>
#include <string>

class AssetManager
{
public:
    AssetManager(std::string const&);
    
    GLuint getTexture(std::string const&) const;
    GLuint getShader(std::string const&) const;
    Model* getModel(std::string const&) const;

    void loadAssets(std::string const&);

private:
    std::map<std::string, GLuint> textures;
    std::map<std::string, GLuint> shaders;
    std::map<std::string, Model*> models;
};

#endif //ASSETMANAGER_H
