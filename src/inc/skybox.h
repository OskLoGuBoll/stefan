#ifndef SKYBOX_H
#define SKYBOX_H

#include "extModel.h"
#include <string>
#include <vector>

class Skybox : public ExtModel
{
public:
    Skybox();
    Skybox(std::string);
    Skybox(Skybox const&) = delete;
    Skybox(Skybox &&) = delete;
    ~Skybox() = default;

    Skybox& operator=(Skybox const&) = default;
    Skybox& operator=(Skybox &&) = default;

    GLuint getCubeMapTexture();
    GLuint setCubeMapTexture(std::string);

    void draw(mat4, mat4, mat4);

private:
    Model* skyboxModel;
    std::string basePath;
    GLuint cubeMapTexture, nolight;
};

#endif //SKYBOX_H
