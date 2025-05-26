#include "demo.h"
#include "world.h"

#include "MicroGlut.h"
#include "pointCloud.h"
#include "fluid.h"

#include <iostream>

#define DELTA_T 8.0

Demo::Demo()
: assets{"assets/"}, world{assets}, keyDown{},
    mouseMovedVec{}, mouseScrollLength{},
    worldTime{}, elapsedTime{}
{
	dumpInfo();

	// GL inits

	printError("GL inits");
    instance = this;
	glutKeyboardFunc(keyboardDownWrapper);
	glutKeyboardUpFunc(keyboardUpWrapper);
	glutMouseFunc(mouseClickWrapper);
	glutPassiveMotionFunc(mouseMovedWrapper);
    glutLogicFunc(logicWrapper);
    glutDisplayFunc(displayWrapper);

	glutRepeatingTimer(DELTA_T);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEBUG_OUTPUT);

    world.addPointCloud("fluid1", std::make_unique<Fluid>(PointCloud{"assets/models/teapot.obj", 60}, Fluid::FluidShaders{
                                                          assets.getShader("depth"),
                                                          assets.getShader("thickness"),
                                                          assets.getShader("blur"),
                                                          assets.getShader("fluidnormals"),
                                                          assets.getShader("composite"),
                                                          assets.getShader("balls"),
                                                          assets.getShader("fluid"),
                                                          }));
}

void Demo::run()
{
    glutHideCursor();
    glutMainLoop();
}

void Demo::keyboardUp(unsigned char key, int x, int y)
{
    keyDown[key] = false;
	keyDown[key+32] = false; 
}

void Demo::keyboardDown(unsigned char key, int x, int y)
{
	keyDown[key] = true;
	keyDown[key+32] = true; // If key is uppercase also register lowercase
}

void Demo::mouseClick(int button, int state, int x, int y)
{
	switch (button)
	{
        case 4:
            mouseScrollLength = std::max(0.0, mouseScrollLength-0.000001);break;
        case 3:
            mouseScrollLength += 0.000001;break;
	}
}

void Demo::mouseMoved(int x, int y)
{
    GLint viewport[4] {};
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLint width {viewport[2]};
    GLint height {viewport[3]};
	mouseMovedVec.x += x-width/2;
	mouseMovedVec.y += y-height/2;
	glutWarpPointer(width/2,height/2);
}

void Demo::logic()
{
    float dt {mouseScrollLength * (glutGet(GLUT_ELAPSED_TIME)-elapsedTime)};
	worldTime += dt;
	elapsedTime = static_cast<GLuint>(glutGet(GLUT_ELAPSED_TIME));

    world.getCamera().handleInput(mouseMovedVec, keyDown.data());
    // This is just a test
    world.update(dt);
    mouseMovedVec = vec2{0,0};
}

void Demo::display(void)
{
	printError("pre display");
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world.draw();

	printError("display");
	glutSwapBuffers();
}

void Demo::keyboardDownWrapper(unsigned char key, int x, int y)
{
    if (instance) instance->keyboardDown(key, x, y);
}

void Demo::keyboardUpWrapper(unsigned char key, int x, int y)
{
    if (instance) instance->keyboardUp(key, x, y);
}

void Demo::mouseClickWrapper(int button, int state, int x, int y)
{
    if (instance) instance->mouseClick(button, state, x, y);
}

void Demo::mouseMovedWrapper(int x, int y)
{
    if (instance) instance->mouseMoved(x, y);
}

void Demo::logicWrapper()
{
    if (instance) instance->logic();
}

void Demo::displayWrapper()
{
    if (instance) instance->display();
}
