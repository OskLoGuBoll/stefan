#include "world.h"
#include "GL_utilities.h"
#include "lights.h"
#include <iostream>

World::World()
: terrain{}, skybox{}, objects{}, camera{}, program{}, nolight{}
{
	program = loadShaders("assets/shaders/lab4-2.vert", "assets/shaders/lab4-2.frag");
	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

	printError("init arrays");
}

Camera& World::getCamera()
{
    return camera;
}

std::vector<ExtModel> const& World::getObjects() const
{
    return objects;
}
