#ifndef CAMERA_H
#define CAMERA_H

#include "VectorUtils4.h"
#include "MicroGlut.h"

class Camera
{
public:
    Camera();
    Camera(Camera const&) = default;
    Camera(Camera &&) = default;
    ~Camera() = default;

    Camera& operator=(Camera const&) = default;
    Camera& operator=(Camera &&) = default;

    void handleInput(vec2 const&, bool const*);

    mat4 getWorldToCamera();
    vec4 getPosition();

private:
    void updatePosition(bool const*);

    mat4 pitchMatrix;
    mat4 yawMatrix;
    vec4 position;

    float pitch;
    float yaw;
    
    float movementSpeed;
    float sensitivity;
};

#endif //CAMERA_H
