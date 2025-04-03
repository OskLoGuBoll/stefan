#include "world.h"

World::World()
: terrain{}, skybox{}
{}

World::World(ExtModel* model, Skybox* skybox)
: terrain{model}, skybox{skybox}
{}

void World::draw()
{
}
