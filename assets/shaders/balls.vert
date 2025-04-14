#version 150

in vec3 in_position;
in float in_radius;
in vec4 in_color;

out vec4 color;
out float radius;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

void main(void)
{
    // Calculate point size based on distance from camera to maintain consistent size
    vec4 worldPos = modelToWorld * vec4(in_position, 1.0);
    vec4 cameraPos = worldToCamera * worldPos;
    gl_Position = cameraToView * cameraPos;
    
    // Perspective-correct point size calculation
    float distance = length(cameraPos.xyz);
    gl_PointSize = in_radius / distance; // Adjust 100.0 to control size scaling
    
    color = in_color;
    radius = in_radius;
}
