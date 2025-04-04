#include "MicroGlut.h"
#include "demo.h"

int main(int argc, char *argv[])
{
    Demo demo1{argc, argv};

    demo1.run();
    return 0;
}