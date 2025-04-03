#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "LoadTGA.h"
#include "LittleOBJLoader.h"

#include <map>
#include <string>

class AssetManager
{
public:
    AssetManager();
    
    GLuint getTex(std::string const&);

    void loadAssets(std::string const&);

private:
    std::map<std::string, GLuint> textures;
};

#endif //ASSETMANAGER_H
