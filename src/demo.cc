#include "demo.h"
#include "world.h"
#include "MicroGlut.h"

#define SCREEN_WIDTH 2560.0
#define SCREEN_HEIGHT 1440.0

Demo* Demo::instance = nullptr;

Demo::Demo(int argc, char *argv[])
{
    instance = this;
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow ("Computer graphics");
	glutKeyboardFunc(keyboardDownWrapper);
	glutKeyboardUpFunc(keyboardUpWrapper);
	glutMouseFunc(mouseClickWrapper);
	glutPassiveMotionFunc(mouseMovedWrapper);

    World world1{mouseMovedVec, mouseScrollLength, keyDown};
}

void Demo::run()
{
    glutMainLoop();
    glutHideCursor();
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
	mouseMovedVec.x += x-SCREEN_WIDTH/2;
	mouseMovedVec.y += y-SCREEN_HEIGHT/2;
	glutWarpPointer(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
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