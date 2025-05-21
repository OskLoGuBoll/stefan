#define MAIN

#include "demo.h"

#define SCREEN_WIDTH 1920.0
#define SCREEN_HEIGHT 1080.0

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow ("STEFAN");

    Demo demo1{};

    demo1.run();
    return 0;
}
