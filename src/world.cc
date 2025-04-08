#include "world.h"
#include "GL_utilities.h"
#include <iostream>

#include "lights.h"

World::World(AssetManager const& assets)
: terrain{assets.getModel("Ground"), assets.getShader("lab4-2")}, skybox{assets}, objects{}, camera{}, program{}, nolight{}
{
    // Just for test
    addObject("ball2", std::make_unique<ExtModel>(assets));
}

Camera& World::getCamera()
{
    return camera;
}

std::unordered_map<std::string,std::unique_ptr<ExtModel>> const& World::getObjects() const
{
    return objects;
}

void World::addObject(const std::string& objectName, std::unique_ptr<ExtModel> object) {
    objects[objectName] = std::move(object);; // overwrites if key exists
}

ExtModel* World::getObject(const std::string& objectName) {
    auto object = objects.find(objectName);
    return (object != objects.end()) ? object->second.get() : nullptr;
}

void World::draw() const
{
	mat4 worldToCamera {camera.getWorldToCamera()};
    mat4 cameraToView {camera.getProjectionMat()};

    skybox.draw(worldToCamera, cameraToView);
    terrain.draw(worldToCamera, cameraToView);

    for (const auto& object : objects)
    {
        GLuint program = object.second->getShader();

        glUseProgram(program);
        glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
        glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
        glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

        object.second->draw(worldToCamera, cameraToView);
    }
}
