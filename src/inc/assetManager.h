#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "LoadTGA.h"
#include "LittleOBJLoader.h"
#include "world.h"

#include <map>
#include <string>

class AssetManager
{
public:
    AssetManager();
    
    GLuint getTexture(std::string const&);
    GLuint getShader(std::string const&);

    void loadAssets(std::string const&);

    void draw(World&);

private:
    std::map<std::string, GLuint> textures;
    std::map<std::string, GLuint> shaders;
};

#endif //ASSETMANAGER_H
