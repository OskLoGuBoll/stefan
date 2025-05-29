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
    float dist = length(coord);

    float alpha = smoothstep(1.0, 0.95, dist);

    // Discard fully transparent fragments
    if (alpha <= 0.0) discard;

    vec3 normal;
    normal.x = coord.x;
    normal.y = -coord.y;
    normal.z = sqrt(1.0 - dot(coord, coord));

    vec4 surfacePos = cameraToView * vec4(positionInView.xyz + normal, 1);
    
    float depth = surfacePos.z / surfacePos.w;
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * 1.0) / (20.0 + 1.0 - z * (20.0 - 1.0));
    gl_FragDepth = depth*0.5+0.5;

    out_color = vec4(linearDepth, 0, 0, 1);
}
