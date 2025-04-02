#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "LoadTGA.h"

#include <map>
#include <string>

class AssetManager
{
public:
    AssetManager();

private:
    std::map<std::string, TextureData> textures;
};

#endif //ASSETMANAGER_H
