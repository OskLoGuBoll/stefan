#include "fluid.h"

#include <random>
#include <GL/glext.h>

Fluid::Fluid(PointCloud& cloud, GLuint shader, GLuint screenShader, GLuint computeShader)
: PointCloud{cloud}, shader{shader}, screenShader{screenShader}, computeShader{computeShader},
    vao{}, posBuffer{}, velBuffer{}
{
    initBuffers();
}

Fluid::Fluid(PointCloud&& cloud, GLuint shader, GLuint screenShader, GLuint computeShader)
: PointCloud{cloud}, shader{shader}, screenShader{screenShader}, computeShader{computeShader},
    vao{}, posBuffer{}, velBuffer{}
{
    initBuffers();
}

void Fluid::draw(mat4 const& worldToCamera, mat4 const& cameraToView, vec2 const& frustumBounds)
{
    
    // first pass
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.4f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);
    

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

    // second pass
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(screenShader);
    glUniform1i(glGetUniformLocation(screenShader, "screenTexture"), 0);

    glBindVertexArray(quadVAO);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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

    
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    
    // generate texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2560, 1440, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    
    // attach it to currently bound framebuffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);  
    
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo); 
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 2560, 1440);  
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    initScreenSpaceQuad();
}

void Fluid::initScreenSpaceQuad()
{
    glGenVertexArrays(1, &quadVAO);
    unsigned int quadVBO;
    glGenBuffers(1, &quadVBO);

    float quadVertices[] = {
        // positions    // texCoords
        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
        -1.0f, -1.0f,   0.0f, 0.0f,  // bottom-left
         1.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
    
        -1.0f,  1.0f,   0.0f, 1.0f,  // top-left
         1.0f, -1.0f,   1.0f, 0.0f,  // bottom-right
         1.0f,  1.0f,   1.0f, 1.0f   // top-right
    };

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    GLint loc {glGetAttribLocation(screenShader, "in_Position")};

    // Position attribute
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    loc = glGetAttribLocation(screenShader, "in_TexCoords");
    // TexCoord attribute
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}
