#ifndef DEMO_H
#define DEMO_H

#include "VectorUtils4.h"
#include <vector>

class Demo
{
public:

    Demo(int argc, char *argv[]);
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

    static void keyboardDownWrapper(unsigned char, int, int);
    static void keyboardUpWrapper(unsigned char, int, int);
    static void mouseClickWrapper(int , int, int, int);
    static void mouseMovedWrapper(int x, int y);

    static Demo* instance;

    std::vector<bool> keyDown;
    vec2 mouseMovedVec;
    float mouseScrollLength;
};

#endif