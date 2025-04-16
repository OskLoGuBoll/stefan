#include "extModel.h"

ExtModel::ExtModel(AssetManager const& assets, vec3 const& position)
: position{position}, texIDs{}, model{assets.getModel("groundsphere")}, shader{assets.getShader("default")}
{}

ExtModel::ExtModel(Model* model, GLuint const shader, vec3 const& position)
: position{position}, texIDs{}, model{model}, shader{shader}
{}

void ExtModel::draw(mat4 const& worldToCamera, mat4 const& cameraToView) const
{
    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "cameraToView"), 1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(shader, "worldToCamera"), 1, GL_TRUE, worldToCamera.m);
                       
    GLfloat specularExponent = 70;
    vec3 kValues = vec3{0.1,0.9,0.7};
    glUniform1f(glGetUniformLocation(shader, "specularStrength"), specularExponent);
    glUniform3fv(glGetUniformLocation(shader, "kValues"), 1, &kValues.x);
                       
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelToWorld"), 1, GL_TRUE, getModelToWorld().m);                                
    DrawModel(model, shader, "in_Position", "in_Normal", "in_TextureCoord");
}

void ExtModel::setShader(GLuint const shaderIn)
{
    shader = shaderIn;
}

GLuint ExtModel::getShader() const
{
    return shader;
}

mat4 ExtModel::getModelToWorld() const
{
    return T(position.x, position.y, position.z);
}

Model* ExtModel::getModel() const
{
    return model;
}

// Implemented just for test.
void ExtModel::setPosition(double const speed)
{
    position.x += speed;
}
