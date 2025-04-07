#ifndef SKYBOX_H
#define SKYBOX_H

#include "extModel.h"

#include "LoadTGA.h"
#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "assetManager.h"

#include <string>
#include <vector>

class Skybox : public ExtModel
{
public:
    Skybox(AssetManager const&);
    Skybox(AssetManager const&, std::string const&);
    Skybox(Skybox const&) = default;
    Skybox(Skybox &&) = default;
    ~Skybox() = default;

    Skybox& operator=(Skybox const&) = default;
    Skybox& operator=(Skybox &&) = default;

    GLuint getCubeMapTexture() const;
    GLuint setCubeMapTexture(AssetManager const&, std::string const&);

    void draw(mat4, mat4 const&) const;

private:
    GLuint cubeMapTexture;
};

#endif //SKYBOX_H
