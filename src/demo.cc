#include "demo.h"
#include "world.h"

#include "MicroGlut.h"
#include "pointCloud.h"

#include <iostream>

#define DELTA_T 8.0

Demo::Demo(float width, float height)
: assets{"assets/"}, world{assets}, keyDown{}, windowSize{width, height},
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

    glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_DEBUG_OUTPUT);

    // Just for test
    world.addObject("ball1",std::make_unique<ExtModel>(assets));

    PointCloud<10000> pc{"assets/models/groundsphere.obj"};
    pc.SaveToFile("assets/pc.xyz");

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
	mouseMovedVec.x += x-windowSize.x/2;
	mouseMovedVec.y += y-windowSize.y/2;
	glutWarpPointer(windowSize.x/2,windowSize.y/2);
}

void Demo::logic()
{
    float dt {mouseScrollLength * (glutGet(GLUT_ELAPSED_TIME)-elapsedTime)};
	worldTime += dt;
	elapsedTime = static_cast<GLuint>(glutGet(GLUT_ELAPSED_TIME));

    world.getCamera().handleInput(mouseMovedVec, keyDown.data());
    mouseMovedVec = vec2{0,0};
}

void Demo::display(void)
{
    if (!glIsEnabled(GL_DEPTH_TEST)) {
        std::cout << "OpenGL not initialized!" << std::endl;
        return;
    }

	printError("pre display");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world.draw();

    // This is just a test
    world.getObject("ball1")->setPosition(0.05);
    world.getObject("ball2")->setPosition(0.01);
    //std::cout<<world.getObject("ball1")->getModelToWorld().m[3]<<std::endl;

	//printf("%d %d \n", nr1, nr2);
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
