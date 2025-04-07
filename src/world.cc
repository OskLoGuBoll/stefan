#include "world.h"
#include "GL_utilities.h"
#include <iostream>

#include "lights.h"

World::World(AssetManager const& assets)
: terrain{assets}, skybox{assets}, objects{}, camera{}, program{}, nolight{}
{}

Camera& World::getCamera()
{
    return camera;
}

std::vector<ExtModel> const& World::getObjects() const
{
    return objects;
}

void World::addObject(ExtModel const& object)
{
    objects.push_back(object);
}

void World::draw() const
{
	mat4 worldToCamera {camera.getWorldToCamera()};
    mat4 cameraToView {camera.getProjectionMat()};

    skybox.draw(worldToCamera, cameraToView);
    terrain.draw(worldToCamera, cameraToView);

    for (auto const& object : objects)
    {
        GLuint program = object.getShader();

        glUseProgram(program);
        glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
        glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
        glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

        object.draw(worldToCamera, cameraToView);
    }

}
