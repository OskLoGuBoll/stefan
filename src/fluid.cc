#include "fluid.h"

#include <random>

Fluid::Fluid(PointCloud& cloud, GLuint shader)
: PointCloud{cloud}, shader{shader}, vao{}, vb{}, rb{}, cb{}
{
    initArrays();
}

Fluid::Fluid(PointCloud&& cloud, GLuint shader)
: PointCloud{cloud}, shader{shader}, vao{}, vb{}, rb{}, cb{}
{
    initArrays();
}

void Fluid::draw(mat4 const& worldToCamera, mat4 const& cameraToView)
{
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "cameraToView"),
                       1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(shader, "worldToCamera"),
                       1, GL_TRUE, worldToCamera.m);
    glUniformMatrix4fv(glGetUniformLocation(shader, "modelToWorld"),
                       1, GL_TRUE, T(centerPosition.x, centerPosition.y, centerPosition.z).m);                                

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, pointCloud.size());
    glBindVertexArray(0);
}

void Fluid::update(float const dt)
{
    std::random_device rd {};
    std::mt19937 gen{rd()};
    std::uniform_real_distribution<float> distrib {-1.f, 1.f};

    for (auto& particle : pointCloud)
    {
        particle += vec3{distrib(gen)*dt, distrib(gen)*dt, distrib(gen)*dt};
    }

    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferSubData(GL_ARRAY_BUFFER, 
                   0, 
                   pointCloud.size() * sizeof(vec3),
                   pointCloud.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GLuint Fluid::getShader() const
{
    return shader;
}

void Fluid::initArrays()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vb);
    glGenBuffers(1, &rb);
    glGenBuffers(1, &cb);
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vb);
    glBufferData(GL_ARRAY_BUFFER, 
         pointCloud.size() * sizeof(vec3),
         pointCloud.data(), 
         GL_DYNAMIC_DRAW);

    GLint loc {};

	loc = glGetAttribLocation(shader, "in_position");
    if (loc >= 0)
    {
        glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0); 
        glEnableVertexAttribArray(loc);
    }

    glBindBuffer(GL_ARRAY_BUFFER, rb);
    glBufferData(GL_ARRAY_BUFFER, 
         radii.size() * sizeof(float),
         radii.data(), 
         GL_STATIC_DRAW);

	loc = glGetAttribLocation(shader, "in_radius");
    if (loc >= 0)
    {
        glVertexAttribPointer(loc, 1, GL_FLOAT, GL_FALSE, 0, 0); 
        glEnableVertexAttribArray(loc);
    }

    glBindBuffer(GL_ARRAY_BUFFER, cb);
    glBufferData(GL_ARRAY_BUFFER, 
         colors.size() * sizeof(vec4),
         colors.data(), 
         GL_STATIC_DRAW);

	loc = glGetAttribLocation(shader, "in_color");
    if (loc >= 0)
    {
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0); 
        glEnableVertexAttribArray(loc);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
