#ifndef DEMO_H
#define DEMO_H

#include "VectorUtils4.h"
#include <array>
#include <iostream>

#include "world.h"
#include "assetManager.h"

class Demo
{
public:

    Demo();
    ~Demo() = default;
    Demo(Demo const&) = delete;
    Demo(Demo &&) = delete;

    Demo& operator=(Demo const&) = delete;
    Demo& operator=(Demo &&) = delete;

    void run();

private:
    void keyboardDown(unsigned char, int, int);
    void keyboardUp(unsigned char, int, int);
    void mouseClick(int , int, int, int);
    void mouseMoved(int, int);
    void logic();
    void display();

    static void keyboardDownWrapper(unsigned char, int, int);
    static void keyboardUpWrapper(unsigned char, int, int);
    static void mouseClickWrapper(int , int, int, int);
    static void mouseMovedWrapper(int x, int y);
    static void logicWrapper();
    static void displayWrapper();

    static inline Demo* instance = nullptr;

    AssetManager assets;
    World world;

    std::array<bool, 256> keyDown;
    vec2 mouseMovedVec;
    float mouseScrollLength;
    float worldTime; //In micro-seconds.
    float elapsedTime; //In micro-seconds.
};

#endif
