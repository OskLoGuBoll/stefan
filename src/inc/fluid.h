#ifndef FLUID_H
#define FLUID_H

#include "pointCloud.h"
#include "VectorUtils4.h"

class Fluid : public PointCloud
{
public:
    Fluid(PointCloud&, GLuint, GLuint, GLuint, GLuint);
    Fluid(PointCloud&&, GLuint, GLuint, GLuint, GLuint);

    void draw(mat4 const&, mat4 const&);
    void update(float const);
    GLuint getShader() const;

private:
    void initBuffers();
    void initSSBOs();
    void initVAOs();
    void initFBO();

    GLuint shader;
    GLuint depthThicknessShader;
    GLuint normalShader;
    GLuint computeShader;

    GLuint particleVAO;
    GLuint frameVAO;
    GLuint fbo;

    GLuint posBuffer;
    GLuint velBuffer;

    GLuint depthThicknessTex;
    GLuint normalTex;
};

#endif // FLUID_H
