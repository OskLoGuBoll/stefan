#include "assetManager.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <array>

#include <iostream>
#include <array>

#include "ground.h"

namespace fs = std::filesystem;

AssetManager::AssetManager(std::string const& path)
: textures{}, shaders{}, models{}
{
    loadAssets(path);
}

void AssetManager::loadAssets(std::string const& assetPath)
{
    std::cout << "<<< Read files: " << std::endl;
    for (auto const& file : fs::directory_iterator(assetPath))
    {
        std::string filename {file.path().string()};

        if (filename == assetPath + "textures")
        {
            for (auto const& texture : fs::directory_iterator(filename))
            {
                std::string key {texture.path().stem().string()};
                if (fs::is_regular_file(texture) && key.find("hm") != std::string::npos)
                {
                    TextureData terrain {};
                    LoadTGATextureData(texture.path().string().c_str(), &terrain);
                    models[key] = GenerateTerrain(&terrain);
                }
                else if (fs::is_regular_file(texture))
                {
                    LoadTGATextureSimple(texture.path().string().c_str(), &textures[key]);
                }
                else
                {
                    std::array<std::string, 6> faces {
                        (texture.path().string() + "/right.tga"),
                        (texture.path().string() + "/left.tga"),
                        (texture.path().string() + "/bottom.tga"),
                        (texture.path().string() + "/top.tga"),
                        (texture.path().string() + "/front.tga"),
                        (texture.path().string() + "/back.tga")
                    };

                    std::array<char const*, 6> cFaces {};
                    for (int i {}; i < 6 ; ++i)
                    {
                        cFaces[i] = faces[i].data();
                    }

                    textures[key] = LoadTGACubemap(cFaces.data());
                    if (glIsTexture(textures[key]) == GL_FALSE) {
                        std::cout << "Failed to load cubemap textures!" << std::endl;
                        exit(EXIT_FAILURE);
                    }
                }
            }
        } 
        else if (filename == assetPath + "shaders")
        {
            for (auto const& shader : fs::directory_iterator(filename))
            {
                std::string key {shader.path().stem().string()};

                if (fs::is_regular_file(shader) && shaders[key] == 0) {
                    shaders[key] = loadShaders((assetPath + "shaders/" + key + ".vert").c_str(),
                                               (assetPath + "shaders/" + key + ".frag").c_str());
                }
            }
        }
        else if (filename == assetPath + "models")
        {
            for (auto const& model : fs::directory_iterator(filename))
            {
                std::string key {model.path().stem().string()};
                std::string modelPath {model.path().string()};
                
                if (fs::is_regular_file(model) && key.find("copy") != std::string::npos)
                {

                }
                else if (fs::is_regular_file(model))
                {
                    models[key] = LoadModel(modelPath.c_str());
                }
            }
        }
    }
    std::cout << "Stop read files >>> " << std::endl;
}


GLuint AssetManager::getTexture(std::string const& key) const
{
    return textures.at(key);
}

GLuint AssetManager::getShader(std::string const& key) const
{
    return shaders.at(key);
}

Model* AssetManager::getModel(std::string const& key) const
{
    return models.at(key);
}
