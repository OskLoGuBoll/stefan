#include "extModel.h"

#include "LoadTGA.h"
#include "ground.h"

ExtModel::ExtModel()
: position{}, texIDs{}, model{}
{
    // THIS IS TEMPORARY //
    //////////////////////////////////////////////////////
    TextureData tex;
    LoadTGATextureData("assets/textures/fft-terrain.tga", &tex);
    model = GenerateTerrain(&tex);
}

ExtModel::ExtModel(Model* model)
: position{}, texIDs{}, model{model}
{}

ExtModel::~ExtModel()
{
    delete model;
}

void ExtModel::draw(mat4 const& modelToWorld, mat4 const& worldToCamera, mat4 const& cameraToView, GLuint program)
{
    // THIS IS TEMPORARY //
    //////////////////////////////////////////////////////
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "cameraToView"), 1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(program, "worldToCamera"), 1, GL_TRUE, worldToCamera.m);
                       
    GLfloat specularExponent = 70;
    vec3 kValues = vec3{0.1,0.9,0.7};
    glUniform1f(glGetUniformLocation(program, "specularStrength"), specularExponent);
    glUniform3fv(glGetUniformLocation(program, "kValues"), 1, &kValues.x);
                       
    glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);                                
    DrawModel(model, program, "in_Position", "in_Normal", "in_TextureCoord");
}
