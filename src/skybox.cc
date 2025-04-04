#include "skybox.h"
#include "LoadTGA.h"
#include "GL_utilities.h"

Skybox::Skybox() : skyboxModel{LoadModel("assets/skybox-full-tweaked.obj")}
{
    char const* faces[6];
    faces[0] = "assets/textures/newskyboxtex/right.tga";
	faces[1] = "assets/textures/newskyboxtex/left.tga";
	faces[2] = "assets/textures/newskyboxtex/bottom.tga";
	faces[3] = "assets/textures/newskyboxtex/top.tga";
	faces[4] = "assets/textures/newskyboxtex/front.tga";
	faces[5] = "assets/textures/newskyboxtex/back.tga";
    cubeMapTexture = LoadTGACubemap(faces);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    nolight = loadShaders("assets/shaders/lab4-2sky.vert", "assets/shaders/lab4-2sky.frag");
}

Skybox::Skybox(std::string cubeMapFile) : skyboxModel{LoadModel("assets/skybox-full-tweaked.obj")}
{
    basePath = "assets/textures/" + cubeMapFile + "/";
    char const* faces[6];
    faces[0] = (basePath + "right.tga").c_str();
    faces[1] = (basePath + "left.tga").c_str();
    faces[2] = (basePath + "bottom.tga").c_str();
    faces[3] = (basePath + "top.tga").c_str();
    faces[4] = (basePath + "front.tga").c_str();
    faces[5] = (basePath + "back.tga").c_str();
    cubeMapTexture = LoadTGACubemap(faces);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    nolight = loadShaders("assets/shaders/lab4-2sky.vert", "assets/shaders/lab4-2sky.frag");
}

GLuint Skybox::getCubeMapTexture() {return cubeMapTexture;}

GLuint Skybox::setCubeMapTexture(std::string cubeMapFile)
{
    basePath = "assets/textures/" + cubeMapFile + "/";
    char const* faces[6];
    faces[0] = (basePath + "right.tga").c_str();
    faces[1] = (basePath + "left.tga").c_str();
    faces[2] = (basePath + "bottom.tga").c_str();
    faces[3] = (basePath + "top.tga").c_str();
    faces[4] = (basePath + "front.tga").c_str();
    faces[5] = (basePath + "back.tga").c_str();
    cubeMapTexture = LoadTGACubemap(faces);
    return cubeMapTexture;
}

void Skybox::draw(mat4 modelToWorld, mat4 worldToCamera, mat4 cameraToView)
{
    worldToCamera.m[3] = 0;
	worldToCamera.m[7] = 0;
	worldToCamera.m[11] = 0;

    glUseProgram(nolight);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "cameraToView"), 1, GL_TRUE, cameraToView.m);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "worldToCamera"), 1, GL_TRUE, worldToCamera.m);
	glUniform1i(glGetUniformLocation(nolight, "textureUnitCube"), 0);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);
	glDisable(GL_DEPTH_TEST);
	DrawModel(skyboxModel, nolight, "in_Position", NULL, NULL);
	glEnable(GL_DEPTH_TEST);
}