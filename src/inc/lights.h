vec3 lightSourcesColorsArr[] = { vec3(1.0f, 1.0f, 1.0f), // Red light

                                 vec3(1.0f, 1.0f, 1.0f), // Green light

                                 vec3(1.0f, 1.0f, 1.0f), // Blue light

                                 vec3(1.0f, 1.0f, 1.0f) }; // White light

GLint isDirectional[] = {0,0,0,1};


vec3 lightSourcesDirectionsPositions[] = { vec3(10.0f, 5.0f, 0.0f), // Red light, positional

                                       vec3(0.0f, 5.0f, 10.0f), // Green light, positional

                                       vec3(15.0f, 20.0f, 0.0f), // Blue light along X

                                       vec3(1.0f, 3.0f, 1.0f) }; // White light along Z

GLfloat specularExponent[] = {100.0, 200.0, 60.0};