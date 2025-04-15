#version 150

in vec3 in_position;

out float radius;
out vec4 color;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

uniform float in_radius = 100;

void main(void)
{
    vec4 worldPos = modelToWorld * vec4(in_position, 1.0);
    vec4 cameraPos = worldToCamera * worldPos;
    gl_Position = cameraToView * cameraPos;
    
    // Perspective-correct point size calculation
    float distance = length(cameraPos.xyz);
    gl_PointSize = in_radius / distance; // Adjust 100.0 to control size scaling
    
    radius = in_radius;
    color = vec4(in_position, 1);
}
