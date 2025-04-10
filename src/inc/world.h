#ifndef WORLD_H
#define WORLD_H

#include "extModel.h"
#include "skybox.h"
#include "camera.h"
#include "fluid.h"
#include "assetManager.h"
#include <unordered_map>
#include <memory>

class World
{
public:
    World(AssetManager const&);
    World(World const&) = delete;
    World(World &&) = delete;

    World& operator=(World const&) = delete;
    World& operator=(World &&) = delete;

    Camera& getCamera();
    std::unordered_map<std::string,std::unique_ptr<ExtModel>> const& getObjects() const;
    void addObject(std::string const&, std::unique_ptr<ExtModel>);
    void addPointCloud(std::string const&, std::unique_ptr<Fluid>);
    void draw() const;
    void init(AssetManager const&);

    ExtModel* getObject(const std::string& key);

private:
    ExtModel terrain;
    Skybox skybox;
    std::unordered_map<std::string,std::unique_ptr<ExtModel>> objects;
    std::unordered_map<std::string,std::unique_ptr<Fluid>> fluids;
    Camera camera;
    GLuint program, nolight;
};

#endif //WORLD_H
