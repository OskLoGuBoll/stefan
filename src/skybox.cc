#include "skybox.h"

#include <iostream>
#include <array>
#include <cassert>

Skybox::Skybox() 
: ExtModel{LoadModel("assets/skybox-full-tweaked.obj")},
    basePath{"assets/textures/newskyboxtex/"}, cubeMapTexture{}, nolight{}
{
    assert(glGetString(GL_VENDOR) != nullptr);  // Crash early if missing
    initCubemap();

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    nolight = loadShaders("assets/shaders/lab4-2sky.vert", "assets/shaders/lab4-2sky.frag");
    if (nolight == 0) {  // Assuming 0 indicates failure
        std::cout << "Shader compilation failed!" << std::endl;
    }
}

Skybox::Skybox(std::string cubeMapFile)
: ExtModel{LoadModel("assets/skybox-full-tweaked.obj")},
    basePath{"assets/textures/" + cubeMapFile + "/"}, cubeMapTexture{}, nolight{}
{
    assert(glGetString(GL_VENDOR) != nullptr);  // Crash early if missing
    initCubemap();

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    nolight = loadShaders("assets/shaders/lab4-2sky.vert", "assets/shaders/lab4-2sky.frag");
    if (nolight == 0) {  // Assuming 0 indicates failure
        std::cout << "Shader compilation failed!" << std::endl;
    }
}

GLuint Skybox::getCubeMapTexture() {return cubeMapTexture;}

GLuint Skybox::setCubeMapTexture(std::string cubeMapFile)
{
    basePath = "assets/textures/" + cubeMapFile + "/";
    initCubemap();
    return cubeMapTexture;
}

void Skybox::draw(mat4 const& modelToWorld, mat4 worldToCamera, mat4 const& cameraToView)
{
    // THIS IS TEMPORARY //
    //////////////////////////////////////////////////////
    worldToCamera.m[3] = 0;
	worldToCamera.m[7] = 0;
	worldToCamera.m[11] = 0;

    glUseProgram(nolight);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "cameraToView"), 1, GL_TRUE, cameraToView.m);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "worldToCamera"), 1, GL_TRUE, worldToCamera.m);
	glUniform1i(glGetUniformLocation(nolight, "textureUnitCube"), 0);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);
	glDisable(GL_DEPTH_TEST);
	DrawModel(model, nolight, "in_Position", NULL, NULL);
	glEnable(GL_DEPTH_TEST);
}

void Skybox::initCubemap()
{
    std::array<std::string, 6> faces {
        (basePath + "right.tga"),
        (basePath + "left.tga"),
        (basePath + "bottom.tga"),
        (basePath + "top.tga"),
        (basePath + "front.tga"),
        (basePath + "back.tga")
    };

    std::array<char const*, 6> cFaces {};
    for (int i {}; i < 6 ; ++i)
    {
        cFaces[i] = faces[i].data();
    }

    cubeMapTexture = LoadTGACubemap(cFaces.data());
    if (glIsTexture(cubeMapTexture) == GL_FALSE) {
        std::cout << "Failed to load cubemap textures!" << std::endl;
        exit(EXIT_FAILURE);
    }
}
