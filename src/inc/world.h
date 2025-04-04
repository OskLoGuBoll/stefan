#ifndef WORLD_H
#define WORLD_H

#include "extModel.h"
#include "skybox.h"
#include "camera.h"

class World
{
public:
    World(vec2&, float&, std::vector<bool>&);
    World(World const&) = delete;
    World(World &&) = delete;
    ~World();

    World& operator=(World const&) = delete;
    World& operator=(World &&) = delete;

    void draw();
    void logic();
    void display();
    static void logicWrapper();
    static void displayWrapper();

private:
    static World* instance;
    ExtModel* terrain;
    Skybox* skybox;
    Camera camera;
    float worldTime; //In micro-seconds.
    float elapsedTime; //In micro-seconds.
    vec2& mouseMovedVec;
    float& mouseScrollLength;
    std::vector<bool>& keyDown;
    GLuint program, nolight;
    mat4 modelToWorld, worldToCamera, cameraToView;
    

};

#endif //WORLD_H
