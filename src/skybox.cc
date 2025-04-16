#include "skybox.h"

#include <iostream>
#include <array>
#include <cassert>

Skybox::Skybox(AssetManager const& assets) 
: ExtModel{assets.getModel("skybox-full-tweaked"), assets.getShader("skybox")},
    cubeMapTexture{assets.getTexture("skyboxtexture")}
{
    assert(glGetString(GL_VENDOR) != nullptr);  // Crash early if missing

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
}

Skybox::Skybox(AssetManager const& assets, std::string const& cubeMapFile)
: ExtModel{assets.getModel("skybox-full-tweaked"), assets.getShader("lab4-2sky")},
    cubeMapTexture{assets.getTexture(cubeMapFile)}
{
    assert(glGetString(GL_VENDOR) != nullptr);  // Crash early if missing

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
}

GLuint Skybox::getCubeMapTexture() const {return cubeMapTexture;}

GLuint Skybox::setCubeMapTexture(AssetManager const& assets, std::string const& cubeMapFile)
{
    cubeMapTexture = assets.getTexture(cubeMapFile);
    return cubeMapTexture;
}

void Skybox::draw(mat4 worldToCamera, mat4 const& cameraToView) const
{
    // THIS IS TEMPORARY //
    //////////////////////////////////////////////////////
    worldToCamera.m[3] = 0;
	worldToCamera.m[7] = 0;
	worldToCamera.m[11] = 0;

    glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "cameraToView"), 1, GL_TRUE, cameraToView.m);
	glUniformMatrix4fv(glGetUniformLocation(shader, "worldToCamera"), 1, GL_TRUE, worldToCamera.m);
	glUniform1i(glGetUniformLocation(shader, "textureUnitCube"), 0);
	glUniformMatrix4fv(glGetUniformLocation(shader, "modelToWorld"), 1, GL_TRUE, getModelToWorld().m);
	glDisable(GL_DEPTH_TEST);
	DrawModel(model, shader, "in_Position", NULL, NULL);
	glEnable(GL_DEPTH_TEST);
}
