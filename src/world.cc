#include "world.h"
#include "GL_utilities.h"
#include "lights.h"

#define NEAR 1.0
#define FAR 500.0
#define DELTA_T 8.0 // ~120fps with 8.0, ~60fps with 16.0 as target fps.
#define SCREEN_WIDTH 2560.0
#define SCREEN_HEIGHT 1440.0

World* World::instance = nullptr;

World::World(vec2& mouseMovedVec, float& mouseScrollLength, std::vector<bool>& keyDown)
: mouseMovedVec{mouseMovedVec}, mouseScrollLength{mouseScrollLength}, keyDown{keyDown}
{
    instance = this;
    glutLogicFunc(logicWrapper);
    glutDisplayFunc(displayWrapper);

    Skybox skybox{"newskyboxtex"};


	cameraToView = perspective(M_2_PI, SCREEN_WIDTH/SCREEN_HEIGHT, NEAR, FAR);

	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	printError("GL inits");

	// Load and compile shader
	program = loadShaders("assets/shaders/lab4-2.vert", "assets/shaders/lab4-2.frag");
	
	printError("init shader");

	glutRepeatingTimer(DELTA_T);
	glEnable(GL_DEBUG_OUTPUT);

	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);


	printError("init arrays");

}

void World::draw()
{
}

void World::logic()
{
    camera.handleInput(mouseMovedVec, keyDown);
    mouseMovedVec = vec2{0,0};
	worldTime += mouseScrollLength * (glutGet(GLUT_ELAPSED_TIME)-elapsedTime);
	elapsedTime = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
}

void World::display(void)
{
	printError("pre display");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	worldToCamera = camera.getWorldToCamera();
	modelToWorld = T(0,0,0);

    skybox->draw(modelToWorld, worldToCamera, cameraToView);

	//printf("%d %d \n", nr1, nr2);
	printError("display");
	glutSwapBuffers();
}


void World::logicWrapper()
{
    if (instance) instance->logic();
}

void World::displayWrapper()
{
    if (instance) instance->display();
}
