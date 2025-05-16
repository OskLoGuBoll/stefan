#ifndef FLUID_H
#define FLUID_H

#include "pointCloud.h"
#include "VectorUtils4.h"


class Fluid : public PointCloud
{
public:
    Fluid(PointCloud&, GLuint, GLuint, GLuint, GLuint);
    Fluid(PointCloud&&, GLuint, GLuint, GLuint, GLuint);

    void draw(mat4 const&, mat4 const&, vec2 const&);
    void update(float const);
    GLuint getShader() const;

private:
    void initBuffers();
    void initScreenSpaceQuad();

    GLuint shader;
    GLuint screenShader;
    GLuint computeShader;
    GLuint blurShader;

    GLuint framebufferA;
    GLuint textureColorbufferA;
    GLuint textureDepthbufferA;
    GLuint framebufferB;
    GLuint textureColorbufferB;
    GLuint textureDepthbufferB;

    GLuint vao;
    GLuint quadVAO;
    GLuint posBuffer;
    GLuint velBuffer;
};

#endif // FLUID_H
