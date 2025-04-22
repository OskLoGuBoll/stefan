#include "fluid.h"

#include <random>
#include <GL/glext.h>

Fluid::Fluid(PointCloud& cloud, GLuint shader, GLuint computeShader)
: PointCloud{cloud}, shader{shader}, computeShader{computeShader},
    vao{}, posBuffer{}, velBuffer{}
{
    initBuffers();
}

Fluid::Fluid(PointCloud&& cloud, GLuint shader, GLuint computeShader)
: PointCloud{cloud}, shader{shader}, computeShader{computeShader},
    vao{}, posBuffer{}, velBuffer{}
{
    initBuffers();
}

void Fluid::draw(mat4 const& worldToCamera, mat4 const& cameraToView, vec2 const& frustumBounds)
{
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "cameraToView"),
                       1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(shader, "worldToCamera"),
                       1, GL_TRUE, worldToCamera.m);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelToWorld"),
                       1, GL_TRUE, T(centerPosition.x, centerPosition.y, centerPosition.z).m);
    glUniform1f(glGetUniformLocation(shader, "near"), frustumBounds.x);
    glUniform1f(glGetUniformLocation(shader, "far"), frustumBounds.y);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, pointCloud.size());
    glBindVertexArray(0);
}

void Fluid::update(float const dt)
{
    glUseProgram(computeShader);

    glUniform1f(glGetUniformLocation(computeShader, "dt"), dt);

    glDispatchCompute((pointCloud.size() + 63) / 64, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

GLuint Fluid::getShader() const
{
    return shader;
}

void Fluid::initBuffers()
{
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);

    glGenVertexArrays(1, &vao);
    glCreateBuffers(1, &posBuffer);
    glCreateBuffers(1, &velBuffer);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
         pointCloud.size() * sizeof(vec4),
         pointCloud.data(), 
         GL_DYNAMIC_DRAW);

    std::vector<vec4> velocities(pointCloud.size(), vec4(0.0f));
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
        velocities.size() * sizeof(vec4),
        velocities.data(), 
        GL_DYNAMIC_DRAW);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);

    GLint loc {glGetAttribLocation(shader, "in_position")};
    if (loc >= 0)
    {
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0); 
        glEnableVertexAttribArray(loc);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
