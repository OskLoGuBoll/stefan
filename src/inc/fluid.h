#ifndef FLUID_H
#define FLUID_H

#include "pointCloud.h"
#include "VectorUtils4.h"


class Fluid : public PointCloud
{
public:
    struct FluidShaders
    {
        GLuint depth;
        GLuint thickness;
        GLuint blur;
        GLuint normal;
        GLuint composite;
        GLuint screen;

        GLuint computeShader;
    };

    Fluid(PointCloud&, FluidShaders const&);
    Fluid(PointCloud&&, FluidShaders const&);

    void draw(mat4 const&, mat4 const&, vec2 const&);
    void update(float const);
    FluidShaders getShaders() const;

private:

    void initBuffers();
    void initScreenSpaceQuad();
    void switchFramebuffer();

    FluidShaders shaders;

    GLuint framebufferA;
    GLuint textureColorbufferA;
    GLuint textureDepthbufferA;
    GLuint textureNormalbufferA;
    GLuint framebufferB;
    GLuint textureColorbufferB;
    GLuint textureDepthbufferB;
    GLuint textureNormalbufferB;
    GLuint framebuffer;
    GLuint textureDepthbuffer;
    GLuint textureColorbuffer;
    GLuint textureNormalbuffer;


    GLuint vao;
    GLuint quadVAO;
    GLuint posBuffer;
    GLuint velBuffer;
};

#endif // FLUID_H
