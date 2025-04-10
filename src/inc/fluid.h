#ifndef FLUID_H
#define FLUID_H

#include "pointCloud.h"
#include "VectorUtils4.h"


class Fluid : public PointCloud
{
public:
    Fluid(PointCloud&, GLuint);
    Fluid(PointCloud&&, GLuint);

    void draw(mat4 const&, mat4 const&);
    GLuint getShader();

private:
    void initArrays();

    GLuint shader;
    GLuint vao;
    GLuint vb;
    GLuint rb;
    GLuint cb;
};

#endif // FLUID_H
