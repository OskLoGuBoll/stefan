#include "assetManager.h"

#include <fstream>
#include <sstream>
#include <filesystem>

#include <iostream>

namespace fs = std::filesystem;

AssetManager::AssetManager()
: textures{}, shaders{}
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
                    //LoadTGACubemap(cubemap);
                }

                std::cout << "Key: " << key << ", TextureData: " << textures[key] << std::endl;
            }
        } else if (filename == assetPath + "shaders")
        {
            for (auto const& shader : fs::directory_iterator(filename))
            {
                std::cout << shader.path().string() << std::endl;
                std::string key {shader.path().stem().string()};

                if (fs::is_regular_file(shader) && shaders[key] == 0) {
                    shaders[key] = loadShaders((assetPath + "shaders/" + key + ".vert").c_str(),
                                               (assetPath + "shaders/" + key + ".frag").c_str());
                }

                std::cout << "Key: " << key << ", ShaderData: " << shaders[key] << std::endl;
            }
        }
    }
    std::cout << "Stop read files >>> " << std::endl;
}


GLuint AssetManager::getTexture(std::string const& key)
{
    return textures[key];
}

GLuint AssetManager::getShader(std::string const& key)
{
    return shaders[key];
}

void AssetManager::draw(World& world)
{
    Camera camera {world.getCamera()};
	mat4 worldToCamera {camera.getWorldToCamera()};
    mat4 cameraToView {camera.getProjectionMat()};
    
    
    for (auto const& object : world.getObjects())
    {
        
    }
}
