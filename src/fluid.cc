#include "fluid.h"

#include <random>
#include <GL/glext.h>

Fluid::Fluid(PointCloud& cloud, FluidShaders const& shaders)
: PointCloud{cloud}, shaders{shaders}, vao{}, posBuffer{}, velBuffer{}
{
    initBuffers();
}

Fluid::Fluid(PointCloud&& cloud, FluidShaders const& shaders)
: PointCloud{cloud}, shaders{shaders}, vao{}, posBuffer{}, velBuffer{}
{
    initBuffers();
}

void Fluid::draw(mat4 const& worldToCamera, mat4 const& cameraToView, vec2 const& frustumBounds)
{
    GLint viewport[4] {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint width {viewport[2]};
    GLint height {viewport[3]};
    vec2 u_scale {1.0f/width, 1.0f/height};
    
    // First pass, depth
    switchFramebuffer();
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaders.depth);

    glUniformMatrix4fv(glGetUniformLocation(shaders.depth, "cameraToView"),
                       1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(shaders.depth, "worldToCamera"),
                       1, GL_TRUE, worldToCamera.m);
    glUniformMatrix4fv(glGetUniformLocation(shaders.depth, "modelToWorld"),
                       1, GL_TRUE, T(centerPosition.x, centerPosition.y, centerPosition.z).m);
    glUniform1f(glGetUniformLocation(shaders.depth, "in_radius"), height/2);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, pointCloud.size());
    glBindVertexArray(0);
    
    // Second pass, thickness
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glUseProgram(shaders.thickness);

    glUniformMatrix4fv(glGetUniformLocation(shaders.thickness, "cameraToView"),
                       1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(shaders.thickness, "worldToCamera"),
                       1, GL_TRUE, worldToCamera.m);
    glUniformMatrix4fv(glGetUniformLocation(shaders.thickness, "modelToWorld"),
                       1, GL_TRUE, T(centerPosition.x, centerPosition.y, centerPosition.z).m);
    glUniform1f(glGetUniformLocation(shaders.depth, "in_radius"), height/4);
    
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, pointCloud.size());
    glDisable(GL_BLEND);
    glBindVertexArray(0);
    
    // Third pass, blur
    switchFramebuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaders.blur);
    glBindVertexArray(quadVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glUniform1i(glGetUniformLocation(shaders.blur, "colorBuffer"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureDepthbuffer);
    glUniform1i(glGetUniformLocation(shaders.blur, "screenDepth"), 1);
    glUniform2fv(glGetUniformLocation(shaders.blur, "u_Scale"), 1, &u_scale.x);

    glUniform1f(glGetUniformLocation(shaders.blur, "near"), frustumBounds.x);
    glUniform1f(glGetUniformLocation(shaders.blur, "far"), frustumBounds.y);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Fourth pass, normal calculation
    
    switchFramebuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaders.normal);
    glBindVertexArray(quadVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glUniform1i(glGetUniformLocation(shaders.normal, "colorBuffer"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureDepthbuffer);
    glUniform1i(glGetUniformLocation(shaders.normal, "screenDepth"), 1);
    glUniformMatrix4fv(glGetUniformLocation(shaders.normal, "cameraToView"),
                       1, GL_TRUE, cameraToView.m);
    glUniform2fv(glGetUniformLocation(shaders.normal, "texelSize"), 1, &u_scale.x);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Fifth pass, composite
    
    switchFramebuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaders.composite);
    glBindVertexArray(quadVAO);

    glUniformMatrix4fv(glGetUniformLocation(shaders.composite, "cameraToView"),
    1, GL_TRUE, cameraToView.m);                                                
    glUniformMatrix4fv(glGetUniformLocation(shaders.composite, "worldToCamera"),
        1, GL_TRUE, worldToCamera.m);
    glUniformMatrix4fv(glGetUniformLocation(shaders.composite, "modelToWorld"),
        1, GL_TRUE, T(centerPosition.x, centerPosition.y, centerPosition.z).m);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureNormalbuffer);
    glUniform1i(glGetUniformLocation(shaders.composite, "colorBuffer"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glUniform1i(glGetUniformLocation(shaders.composite, "depthMap"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textureDepthbuffer);
    glUniform1i(glGetUniformLocation(shaders.composite, "screenDepth"), 2);

    glUniformMatrix4fv(glGetUniformLocation(shaders.composite, "worldToCamera"),
                       1, GL_TRUE, worldToCamera.m);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    

    // Sixth pass, render to screen
    switchFramebuffer();
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shaders.screen);
    
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureDepthbuffer);
    glUniform1i(glGetUniformLocation(shaders.screen, "grebuky"), 0);
    glUniform1i(glGetUniformLocation(shaders.screen, "screenDepth"), 1);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    
}

void Fluid::update(float const dt)
{
    glUseProgram(shaders.computeShader);

    glUniform1f(glGetUniformLocation(shaders.computeShader, "dt"), dt);
    glUniform1i(glGetUniformLocation(shaders.computeShader, "numParticles"), pointCloud.size());

    glDispatchCompute((pointCloud.size() + 63) / 64, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

Fluid::FluidShaders Fluid::getShaders() const
{
    return shaders;
}

void Fluid::initCompBuffers()
{
    glCreateBuffers(1, &posBuffer);
    glCreateBuffers(1, &velBuffer);
    glCreateBuffers(1, &forceBuffer);
    glCreateBuffers(1, &densityBuffer);
    glCreateBuffers(1, &pressureBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, posBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
         pointCloud.size() * sizeof(vec4),
         pointCloud.data(), 
         GL_DYNAMIC_DRAW);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "GL error after buffer init pos: " << std::hex << err << std::endl;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velBuffer);
    std::vector<vec4> velocities(pointCloud.size(), vec4(0.0f));
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
        velocities.size() * sizeof(vec4),
        velocities.data(), 
        GL_DYNAMIC_DRAW);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "GL error after buffer init vel: " << std::hex << err << std::endl;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, forceBuffer);
    std::vector<vec4> forces(pointCloud.size(), vec4(0.0f));
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
        forces.size() * sizeof(vec4),
        forces.data(), 
        GL_DYNAMIC_DRAW);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "GL error after buffer init force: " << std::hex << err << std::endl;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, forceBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, densityBuffer);
    std::vector<float> densities(pointCloud.size(), 0.0f);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
        densities.size() * sizeof(float),
        densities.data(), 
        GL_DYNAMIC_DRAW);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "GL error after buffer init density: " << std::hex << err << std::endl;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, densityBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pressureBuffer);
    std::vector<float> pressures(pointCloud.size(), 0.0f);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
        pressures.size() * sizeof(float),
        pressures.data(), 
        GL_DYNAMIC_DRAW);

    err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "GL error after buffer init pressure: " << std::hex << err << std::endl;
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pressureBuffer);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Fluid::initBuffers()
{
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT_SPRITE);

    glGenVertexArrays(1, &vao);

    initCompBuffers();

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, posBuffer);

    GLint loc {glGetAttribLocation(shaders.depth, "in_position")};
    if (loc >= 0)
    {
        glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, 0); 
        glEnableVertexAttribArray(loc);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLint viewport[4] {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint width {viewport[2]};
    GLint height {viewport[3]};
    
    // Framebuffer A
    glGenFramebuffers(1, &framebufferA);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferA);

    // generate color-texture
    glGenTextures(1, &textureColorbufferA);
    glBindTexture(GL_TEXTURE_2D, textureColorbufferA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbufferA, 0);

    // generate normal-texture
    glGenTextures(1, &textureNormalbufferA);
    glBindTexture(GL_TEXTURE_2D, textureNormalbufferA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureNormalbufferA, 0);  
    
    // generate depth-texture
    glGenTextures(1, &textureDepthbufferA);
    glBindTexture(GL_TEXTURE_2D, textureDepthbufferA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthbufferA, 0);
    
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Framebuffer B
    glGenFramebuffers(1, &framebufferB);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferB);

    // generate color-texture
    glGenTextures(1, &textureColorbufferB);
    glBindTexture(GL_TEXTURE_2D, textureColorbufferB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbufferB, 0);  
    
    // generate normal-texture
    glGenTextures(1, &textureNormalbufferB);
    glBindTexture(GL_TEXTURE_2D, textureNormalbufferB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, textureNormalbufferB, 0);  


    // generate depth-texture
    glGenTextures(1, &textureDepthbufferB);
    glBindTexture(GL_TEXTURE_2D, textureDepthbufferB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureDepthbufferB, 0);
    
    glDrawBuffers(2, drawBuffers);

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

    GLint loc {glGetAttribLocation(shaders.composite, "in_Position")};

    // Position attribute
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    loc = glGetAttribLocation(shaders.composite, "in_TexCoords");
    // TexCoord attribute
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Fluid::switchFramebuffer()
{
    if (framebuffer == framebufferB)
    {
        textureColorbuffer = textureColorbufferB;
        textureDepthbuffer = textureDepthbufferB;
        textureNormalbuffer = textureNormalbufferB;

        framebuffer = framebufferA;
    } else {
        textureColorbuffer = textureColorbufferA;
        textureDepthbuffer = textureDepthbufferA;
        textureNormalbuffer = textureNormalbufferA;

        framebuffer = framebufferB;
    }
    return;
}
