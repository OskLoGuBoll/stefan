#ifndef WORLD_H
#define WORLD_H

#include "extModel.h"
#include "skybox.h"
#include "camera.h"
#include "assetManager.h"

class World
{
public:
    World(AssetManager const&);
    World(World const&) = delete;
    World(World &&) = delete;

    World& operator=(World const&) = delete;
    World& operator=(World &&) = delete;

    Camera& getCamera();
    std::vector<ExtModel> const& getObjects() const;
    void addObject(ExtModel const&);
    void draw() const;
    void init(AssetManager const&);

private:
    ExtModel terrain;
    Skybox skybox;
    std::vector<ExtModel> objects;
    Camera camera;
    GLuint program, nolight;
};

#endif //WORLD_H
