#include "assetManager.h"

#include <fstream>
#include <sstream>
#include <filesystem>

#include <iostream>

namespace fs = std::filesystem;

AssetManager::AssetManager()
: textures{}
{}

void AssetManager::loadAssets(std::string const& assetPath)
{
    std::cout << "<<< Read files: " << std::endl;
    for (auto const& file : fs::directory_iterator(assetPath))
    {
        std::string filename {file.path().string()};
        std::cout << filename << std::endl;

        if (filename == assetPath + "textures")
        {
            for (auto const& texture : fs::directory_iterator(filename))
            {
                std::cout << texture.path().string() << std::endl;
                std::string key {texture.path().stem().string()};
                if (fs::is_regular_file(texture)) {
                    LoadTGATextureSimple(texture.path().string().c_str(), &textures[key]);
                }
                else
                {
                    std::cout << "cubemap" << std::endl;
                    char const* cubemap[5] {};
                    LoadTGACubemap(cubemap);
                }

                std::cout << "Key: " << key << ", Data: " << textures[key] << std::endl;
            }
        }
    }
    std::cout << "Stop read files >>> " << std::endl;
}


GLuint AssetManager::getTex(std::string const& key)
{
    return textures[key];
}
