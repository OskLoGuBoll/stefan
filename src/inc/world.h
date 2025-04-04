#ifndef WORLD_H
#define WORLD_H

#include "extModel.h"
#include "skybox.h"
#include "camera.h"

class World
{
public:
    World();
    World(World const&) = delete;
    World(World &&) = delete;

    World& operator=(World const&) = delete;
    World& operator=(World &&) = delete;

    void draw(mat4 const&, mat4 const&, mat4 const&);
    Camera& getCamera();

private:                
    ExtModel terrain;
    Skybox skybox;
    std::vector<ExtModel> objects;
    Camera camera;
    GLuint program, nolight;
};

#endif //WORLD_H
