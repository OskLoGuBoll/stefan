#version 430

in vec4 positionInView;
in float distanceToCam;
in float sphereRadius;

out vec4 out_color;

uniform mat4 cameraToView;

void main(void)
{
    vec2 coord = (gl_PointCoord) * 2.0 -1.0;
    float dist = length(coord);

    // Soft edge with anti-aliasing
    float alpha = smoothstep(1.0, 0.95, dist);

    // Discard fully transparent fragments
    if (alpha <= 0.0) discard;

    vec3 normal;
    normal.x = coord.x;
    normal.y = -coord.y;

    normal.z = sqrt(1.0 - dot(coord, coord));

    normal = normal*0.5+0.5;


    vec4 surfacePos = cameraToView * (positionInView + vec4(normal, 1));
    
    float depth = surfacePos.z / surfacePos.w * 0.5 + 0.5;
    gl_FragDepth = depth;

    float diffuse = max(0, dot(normal, vec3(1, 0, 1)));

    out_color = vec4(vec3(depth), 1);
}
