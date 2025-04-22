#ifndef FLUID_H
#define FLUID_H

#include "pointCloud.h"
#include "VectorUtils4.h"


class Fluid : public PointCloud
{
public:
    Fluid(PointCloud&, GLuint, GLuint);
    Fluid(PointCloud&&, GLuint, GLuint);

    void draw(mat4 const&, mat4 const&, vec2 const&);
    void update(float const);
    GLuint getShader() const;

private:
    void initBuffers();

    GLuint shader;
    GLuint computeShader;

    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint rbo;

    GLuint vao;
    GLuint posBuffer;
    GLuint velBuffer;
};

#endif // FLUID_H
