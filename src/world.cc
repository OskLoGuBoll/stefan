#include "world.h"
#include "GL_utilities.h"
#include "lights.h"
#include <iostream>

#define DELTA_T 8.0 // ~120fps with 8.0, ~60fps with 16.0 as target fps.

World::World()
: terrain{}, skybox{}, objects{}, camera{}, program{}, nolight{}
{
	// Load and compile shader
	program = loadShaders("assets/shaders/lab4-2.vert", "assets/shaders/lab4-2.frag");
	
	printError("init shader");


	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

	printError("init arrays");
}

void World::draw(mat4 const& modelToWorld,
                 mat4 const& worldToCamera,
                 mat4 const& cameraToView)
{
    skybox.draw(modelToWorld, worldToCamera, cameraToView);
    terrain.draw(modelToWorld, worldToCamera, cameraToView, program);
}

Camera& World::getCamera()
{
    return camera;
}

