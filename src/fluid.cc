#include "fluid.h"

#include <random>
#include <GL/glext.h>

Fluid::Fluid(PointCloud& cloud, GLuint shader, GLuint computeShader,
             GLuint depthThicknessShader, GLuint normalShader)
: PointCloud{cloud}, shader{shader}, depthThicknessShader{depthThicknessShader},
    normalShader{normalShader}, computeShader{computeShader}, particleVAO{},
    frameVAO{}, fbo{}, posBuffer{}, velBuffer{}, depthThicknessTex{}, normalTex{}
{
    initBuffers();
}

Fluid::Fluid(PointCloud&& cloud, GLuint shader, GLuint computeShader,
             GLuint depthThicknessShader, GLuint normalShader)
: PointCloud{cloud}, shader{shader}, depthThicknessShader{depthThicknessShader},
    normalShader{normalShader}, computeShader{computeShader}, particleVAO{},
    frameVAO{}, fbo{}, posBuffer{}, velBuffer{}, depthThicknessTex{}, normalTex{}
{
    initBuffers();
}

void Fluid::draw(mat4 const& worldToCamera, mat4 const& cameraToView)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(depthThicknessShader);

    glUniformMatrix4fv(glGetUniformLocation(depthThicknessShader, "cameraToView"),
                       1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(depthThicknessShader, "worldToCamera"),
                       1, GL_TRUE, worldToCamera.m);
    glUniformMatrix4fv(glGetUniformLocation(depthThicknessShader, "modelToWorld"),
                       1, GL_TRUE, T(centerPosition.x, centerPosition.y, centerPosition.z).m);

    glBindVertexArray(particleVAO);
    glDrawArrays(GL_POINTS, 0, pointCloud.size());
    glBindVertexArray(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthThicknessTex);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(frameVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
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

    initSSBOs();
    initVAOs();
    initFBO();

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Fluid::initSSBOs()
{
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

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Fluid::initVAOs()
{
    glGenVertexArrays(1, &particleVAO);
    glBindVertexArray(particleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);

    GLint loc {glGetAttribLocation(shader, "in_position")};
    if (loc >= 0)
    {
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0); 
        glEnableVertexAttribArray(loc);
    }

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
        };
    glGenVertexArrays(1, &frameVAO);
    GLuint frameVBO;
    glGenBuffers(1, &frameVBO);

    glBindVertexArray(frameVAO);
    glBindBuffer(GL_ARRAY_BUFFER, frameVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    loc = glGetAttribLocation(depthThicknessShader, "in_position");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    loc = glGetAttribLocation(depthThicknessShader, "in_textureCoord");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Fluid::initFBO()
{
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    int viewport[4] {};

    glGetIntegerv(GL_VIEWPORT, viewport);
    int width {viewport[2]};
    int height {viewport[3]};

    glGenTextures(1, &depthThicknessTex);
    glBindTexture(GL_TEXTURE_2D, depthThicknessTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthThicknessTex, 0);

    /*
    glGenTextures(1, &normalTex);
    glBindTexture(GL_TEXTURE_2D, normalTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTex, 0);

    /*
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, drawBuffers);
*/
    auto fboStatus {glCheckFramebufferStatus(GL_FRAMEBUFFER)};
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer error: " << fboStatus << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
