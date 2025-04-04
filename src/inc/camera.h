#ifndef CAMERA_H
#define CAMERA_H

#include "VectorUtils4.h"
#include "MicroGlut.h"
#include <vector>

class Camera
{
public:
    Camera();
    Camera(Camera const&) = default;
    Camera(Camera &&) = default;
    ~Camera() = default;

    Camera& operator=(Camera const&) = default;
    Camera& operator=(Camera &&) = default;

    void handleInput(vec2 const&, std::vector<bool>&);

    mat4 getWorldToCamera();
    vec3 getPosition();

private:
    void updatePosition(std::vector<bool>&);

    mat4 pitchMatrix;
    mat4 yawMatrix;
    vec3 position;

    float pitch;
    float yaw;
    
    float movementSpeed;
    float sensitivity;
};

#endif //CAMERA_H
