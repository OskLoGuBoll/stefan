#version 430

in vec4 positionInView;
in float distanceToCam;
in float sphereRadius;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_Normal;

uniform mat4 cameraToView;

void main(void)
{
    vec2 coord = (gl_PointCoord) * 2.0 -1.0;

    float r2 = dot(coord, coord);
    if (r2 > 1.0) discard; // discard outside the circle

    vec3 normal;
    normal.x = coord.x;
    normal.y = -coord.y;

    normal.z = sqrt(max(0.0, 1.0 - r2));

    normal = normal * 0.5 + 0.5;

    vec4 surfacePos = cameraToView * vec4(positionInView.xyz + normal, 1);
    
    float depth = surfacePos.z / surfacePos.w;
    gl_FragDepth = depth * 0.5 + 0.5;

    float thickness = 0.01;

    out_color = vec4(0, thickness, 0, 1);
}
