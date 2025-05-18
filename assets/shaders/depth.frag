#version 430

in vec4 positionInView;
in float distanceToCam;
in float sphereRadius;

out vec4 out_color;

uniform mat4 cameraToView;

void main(void)
{
    vec2 coord = gl_PointCoord * 2.0 - 1.0;
    float dist = length(coord);

    // Soft edge with anti-aliasing
    float alpha = smoothstep(0.5, 0.45, dist);

    // Discard fully transparent fragments
    if (alpha <= 0.0) discard;

    vec3 normal;
    normal.xy = coord;
    normal.z = sqrt(1.0 - dot(coord, coord));

    vec4 surfacePos = cameraToView * (positionInView + vec4(normal, 1));
    
    float depth = surfacePos.z / surfacePos.w * 0.5 + 0.5;

    out_color = vec4(vec3(depth), 1);
}
