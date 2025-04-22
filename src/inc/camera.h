#ifndef CAMERA_H
#define CAMERA_H

#include "VectorUtils4.h"
#include "MicroGlut.h"
#include <vector>

class Camera
{
public:
    Camera(vec3 const& = 0);
    Camera(Camera const&) = default;
    Camera(Camera &&) = default;
    ~Camera() = default;

    Camera& operator=(Camera const&) = default;
    Camera& operator=(Camera &&) = default;

    void handleInput(vec2 const&, bool const*);

    mat4 getWorldToCamera() const;
    mat4 getProjectionMat() const;
    vec3 getPosition() const;
    vec2 getFrustumBounds() const;

private:
    void updatePosition(bool const*);

    mat4 pitchMatrix;
    mat4 yawMatrix;
    vec3 position;

    float pitch;
    float yaw;

    float near;
    float far;
    float aspect;
    float fov;
    
    float movementSpeed;
    float sensitivity;
};

#endif //CAMERA_H
