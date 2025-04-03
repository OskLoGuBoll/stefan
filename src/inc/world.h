#ifndef WORLD_H
#define WORLD_H

#include "extModel.h"
#include "skybox.h"

class World
{
public:
    World();
    World(ExtModel*, Skybox*);
    World(World const&) = delete;
    World(World &&) = delete;
    ~World();

    World& operator=(World const&) = delete;
    World& operator=(World &&) = delete;

    void draw();

private:
    ExtModel* terrain;
    Skybox* skybox;
};

#endif //WORLD_H
