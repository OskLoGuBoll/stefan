#version 430

in vec3 in_position;

out vec4 positionInView;
out float distanceToCam;
out float sphereRadius;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

uniform float in_radius = 10;

void main(void)
{
    vec4 worldPos = modelToWorld * vec4(in_position, 1.0);
    positionInView = worldToCamera * worldPos;
    gl_Position = cameraToView * positionInView;

    // Perspective-correct point size calculation
    distanceToCam = length(positionInView.xyz);
    gl_PointSize = in_radius / distanceToCam; // Adjust 100.0 to control size scaling

    sphereRadius = in_radius;
}
