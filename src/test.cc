#define MAIN
#include "GL_utilities.h"
#include "MicroGlut.h"
#include "LittleOBJLoader.h"
#include "LoadTGA.h"
#include "VectorUtils4.h"

#include "lights.h"
#include "ground.h"
#include "camera.h"
#include "world.h"
#include "assetManager.h"

#include <cmath>
#include <algorithm>
#include <iostream>

// Globals
// Data would normally be read from files

#define NEAR 1.0
#define FAR 500.0
#define DELTA_T 8.0 // ~120fps with 8.0, ~60fps with 16.0 as target fps.
#define SCREEN_WIDTH 2560.0
#define SCREEN_HEIGHT 1440.0

float calcHeight(float x, float z);

GLfloat t, worldTime; // In micro seconds.
Model* windWalls;
Model* windRoof;
Model* windBlade;
Model* windBalcony;
Model* ground;
Model* skyBox;
Model* teapot;
Model* ball;

GLuint program, nolight;
mat4 rot, trans, scale, rotMotion, modelToWorld;
mat4 cameraToView;
vec2 mouseMovedVec;
GLfloat mouseScrollLength;
vec4 baseColor;
vec2 ballPos;

Camera camera;

vec4 farPlane;
vec4 leftPlane;
vec4 rightPlane;
vec4 topPlane;
vec4 bottomPlane;

bool keyDown[256];

// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint tex1, tex2;
TextureData ttex; // terrain

const char* faces[5];
GLuint cubemapTexture;

AssetManager assets;

void init(void)
{
    assets.loadAssets("assets/");

	faces[0] = "assets/newskyboxtex/right.tga";
	faces[1] = "assets/newskyboxtex/left.tga";
	faces[3] = "assets/newskyboxtex/top.tga";
	faces[2] = "assets/newskyboxtex/bottom.tga";
	faces[4] = "assets/newskyboxtex/front.tga";
	faces[5] = "assets/newskyboxtex/back.tga";
	cubemapTexture = LoadTGACubemap(faces);

	cameraToView = perspective(M_2_PI, SCREEN_WIDTH/SCREEN_HEIGHT, NEAR, FAR);

	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	printError("GL inits");

	// Load and compile shader
	program = loadShaders("assets/lab4-2.vert", "assets/lab4-2.frag");
	nolight = loadShaders("assets/lab4-2sky.vert", "assets/lab4-2sky.frag");
	printError("init shader");

	skyBox = LoadModel("assets/skybox-full-tweaked.obj");

	ball = LoadModel("assets/groundsphere.obj");
	ballPos = vec2{100,100};

	LoadTGATextureData("assets/fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

	glutRepeatingTimer(DELTA_T);
	glEnable(GL_DEBUG_OUTPUT);

	glUseProgram(program);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, assets.getTex("windBody"));
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	printError("init arrays");
}

void display(void)
{
	printError("pre display");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 worldToCameraSkybox = camera.getWorldToCamera();
	worldToCameraSkybox.m[3] = 0;
	worldToCameraSkybox.m[7] = 0;
	worldToCameraSkybox.m[11] = 0;
	modelToWorld = T(0,0,0);
	glUseProgram(nolight);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "cameraToView"), 1, GL_TRUE, cameraToView.m);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "worldToCamera"), 1, GL_TRUE, worldToCameraSkybox.m);
	glUniform1i(glGetUniformLocation(nolight, "textureUnitCube"), 0);
	glUniformMatrix4fv(glGetUniformLocation(nolight, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);
	glDisable(GL_DEPTH_TEST);
	DrawModel(skyBox, nolight, "in_Position", NULL, NULL);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(program);

	glUniformMatrix4fv(glGetUniformLocation(program, "cameraToView"), 1, GL_TRUE, cameraToView.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "worldToCamera"), 1, GL_TRUE, camera.getWorldToCamera().m);

	GLfloat specularExponent = 70;
	vec3 kValues = vec3{0.1,0.9,0};
	glUniform1i(glGetUniformLocation(program, "textureUnit"), 0); // Texture unit 0
	glUniform1f(glGetUniformLocation(program, "specularStrength"), specularExponent);
	glUniform3fv(glGetUniformLocation(program, "kValues"), 1, &kValues.x);

    vec4 camPos {camera.getPosition()};
	vec2 currentTile = vec2{std::floor(camPos.x/(ttex.width-1)),std::floor(camPos.z/(ttex.width-1))};
	int renderDistance = std::ceil(1.41*FAR/(ttex.width-1));
	int nr1 = 0;
	int nr2 = 0;
	for(int i = -renderDistance; i<renderDistance+1; i++)
		for(int j = -renderDistance; j<renderDistance+1; j++)
		{
			nr2+=1;
			//printf("%f, %f \n", (currentTile.x+i+0.5)*(ttex.width-1), (currentTile.y+j+0.5)*(ttex.width-1));
			if (farPlane.x*((0.5)*(ttex.width-1))+
				farPlane.y*(0)+
				farPlane.z*((0.5)*(ttex.width-1))+
				farPlane.w > -sqrt(farPlane.x*farPlane.x+farPlane.y*farPlane.y+farPlane.z*farPlane.z)*(ttex.width-1)/sqrt(2))
				{
					//modelToWorld = T((0)*(ttex.width-1),0,(0)*(ttex.width-1));

					modelToWorld = T((currentTile.x+i)*(ttex.width-1),0,(currentTile.y+j)*(ttex.width-1));
					glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);
					DrawModel(tm, program, "in_Position", "in_Normal", "in_TextureCoord");
					nr1+=1;
				}
		}
	
	modelToWorld = T(ballPos.x, calcHeight(ballPos.x, ballPos.y), ballPos.y);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorld"), 1, GL_TRUE, modelToWorld.m);
	DrawModel(ball, program, "in_Position", "in_Normal", "in_TextureCoord");


	//printf("%d %d \n", nr1, nr2);
	printError("display");
	glutSwapBuffers();
}

void keyboardUp(unsigned char key, int x, int y)
{
	keyDown[key] = false;
	keyDown[key+32] = false; 
}
void keyboardDown(unsigned char key, int x, int y)
{
	keyDown[key] = true;
	keyDown[key+32] = true; // If key is uppercase also register lowercase
}

void mouseClick(int button, int state, int x, int y)
{
	switch (button)
	{
	case 4:
		mouseScrollLength = std::max(0.0, mouseScrollLength-0.000001);break;
	case 3:
		mouseScrollLength += 0.000001;break;
	}
	return;
}
void mouseMoved(int x, int y)
{
	mouseMovedVec.x += x-SCREEN_WIDTH/2;
	mouseMovedVec.y += y-SCREEN_HEIGHT/2;
	glutWarpPointer(SCREEN_WIDTH/2,SCREEN_HEIGHT/2);
}

float calcHeight(float x, float z)
{
	float height;
	//printf("%f %f \n",x,z);
	x = x < 0 ? (ttex.width-1) + fmod(x,ttex.width-1) : fmod(x,ttex.width-1);
	z = z < 0 ? (ttex.width-1) + fmod(z,ttex.width-1) : fmod(z,ttex.width-1);
	int xQuad = (int)std::floor(x);
	int zQuad = (int)std::floor(z);
	int triangle = x-xQuad + z-zQuad < 1 ? 0 : 1;
	//float height = tm->vertexArray[(xQuad + zQuad * ttex.width)].y;
	//camPos.y = ;
	float h1 = tm->vertexArray[(xQuad + zQuad * (ttex.width))].y;
	float h2; // = tm->vertexArray[(xQuad+1 + zQuad * (ttex.width-1))].y;
	float h3; // = tm->vertexArray[(xQuad + (zQuad+1) * (ttex.width-1))].y;
	float h4; // = tm->vertexArray[(xQuad+1 + (zQuad+1) * (ttex.width-1))].y;
	if (xQuad == ttex.width-2 && zQuad == ttex.width-2)
	{
		h2 = tm->vertexArray[(0 + (zQuad) * (ttex.width))].y;
		h3 = tm->vertexArray[(xQuad + (0) * (ttex.width))].y;
		h4 = tm->vertexArray[(0 + (0) * (ttex.width))].y;
	} else if (xQuad == ttex.width-2)
	{
		h2 = tm->vertexArray[(0 + (zQuad) * (ttex.width))].y;
		h3 = tm->vertexArray[(xQuad + (zQuad+1) * (ttex.width))].y;
		h4 = tm->vertexArray[(0 + (zQuad+1) * (ttex.width))].y;
	} else if (zQuad == ttex.width-2)
	{
		h2 = tm->vertexArray[(xQuad+1 + (zQuad) * (ttex.width))].y;
		h3 = tm->vertexArray[(xQuad + (0) * (ttex.width))].y;
		h4 = tm->vertexArray[(xQuad+1 + (0) * (ttex.width))].y;
	} else 
	{
		h2 = tm->vertexArray[(xQuad+1 + zQuad * (ttex.width))].y;
		h3 = tm->vertexArray[(xQuad + (zQuad+1) * (ttex.width))].y;
		h4 = tm->vertexArray[(xQuad+1 + (zQuad+1) * (ttex.width))].y;
	}

	if (triangle == 0) 
	{
		//printf("tr0");
		height = ((h2-h1)*(x-xQuad) + (h3-h1)*(z-zQuad)) + h1;
	} else {
		//printf("tr1");
		height = ((h4-h3)*(x-xQuad)+h3 + (h4-h2)*(z-zQuad)+h2) - h4;
	}

	//camPos.y = height+2;

	//printf("\n%d %d %f %f \n", xQuad, zQuad, x, z);
	return height;
}

void calcFrustumPlanesFLR()
{
	mat4 frustumInWorld = cameraToView*camera.getWorldToCamera();
	frustumInWorld = inverse(frustumInWorld);

	farPlane = frustumInWorld*vec4{0,0,-1,1};
	leftPlane = frustumInWorld*vec4{1,0,0,1};
	rightPlane = frustumInWorld*vec4{-1,0,0,1};

    //vec4 camPos = camera.getPosition();
    //printf("%f, %f, %f \n", camPos.x, camPos.y, camPos.z);
	//printf("%f, %f, %f, %f \n", farPlane.x, farPlane.y, farPlane.z, farPlane.w);
}

void logic(void)
{
	camera.handleInput(mouseMovedVec, keyDown);
    mouseMovedVec = vec2{0,0};
	worldTime += mouseScrollLength * (glutGet(GLUT_ELAPSED_TIME)-t);
	t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	//calcHeight(camPos.x, camPos.z);
	//ballPos.y = 50*sin(worldTime*0.001)+30;
	ballPos.x = 10*cos(worldTime*0.001);
	calcFrustumPlanesFLR();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow ("Computer graphics");
	glutDisplayFunc(display);
	glutLogicFunc(logic);
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouseClick);
	glutPassiveMotionFunc(mouseMoved);
	init();
	glutHideCursor();
	glutMainLoop();
	
	return 0;
}
