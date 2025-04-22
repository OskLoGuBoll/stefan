#include "world.h"
#include "GL_utilities.h"
#include <iostream>

#include "lights.h"

World::World(AssetManager const& assets)
: terrain{assets.getModel("plane"), assets.getShader("default"), std::vector<GLuint>{assets.getTexture("aaa")}}, skybox{assets}, objects{}, fluids{}, camera{}, program{}, nolight{}
{
    // Just for test
}

Camera& World::getCamera()
{
    return camera;
}

std::unordered_map<std::string,std::unique_ptr<ExtModel>> const& World::getObjects() const
{
    return objects;
}

void World::addObject(std::string const& objectName, std::unique_ptr<ExtModel> object)
{
    objects[objectName] = std::move(object); // overwrites if key exists
}

void World::addPointCloud(std::string const& objectName, std::unique_ptr<Fluid> object)
{
    fluids[objectName] = std::move(object);
}

ExtModel* World::getObject(std::string const& objectName) {
    auto object = objects.find(objectName);
    return (object != objects.end()) ? object->second.get() : nullptr;
}

void World::draw() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	mat4 worldToCamera {camera.getWorldToCamera()};
    mat4 cameraToView {camera.getProjectionMat()};
    vec2 frustumBounds {camera.getFrustumBounds()};

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox.draw(worldToCamera, cameraToView);    
    
    GLuint program {terrain.getShader()};
    glUseProgram(program);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);
    
    terrain.draw(worldToCamera, cameraToView);
    
    for (auto const& object : objects)
    {
        object.second->draw(worldToCamera, cameraToView);
    }
    for (auto const& fluid : fluids)
    {
        fluid.second->draw(worldToCamera, cameraToView, frustumBounds);
    }
}

void World::update(float const dt)
{
    for (auto const& fluid : fluids)
    {
        fluid.second->update(dt);
    }
}
