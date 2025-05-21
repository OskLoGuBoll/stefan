#version 430

layout(std430, binding = 0) buffer posBuffer {
    vec4 particles[];
};

in vec4 positionInView;
in float distanceToCam;
in float sphereRadius;

out vec4 out_color;

uniform mat4 cameraToView;
uniform mat4 worldToCamera;
uniform float far;
uniform int particleCount;

float sampleDensity(vec3 pos)
{
    float density = 0;

    for (int i = 0; i < particleCount; i++)
    {
        vec3 particlePos_i = vec3(worldToCamera * particles[i]);
        float dist = distance(pos, particlePos_i);
        float falloff = max(0, dist);

        density += falloff;
    }

    return min(1, density);
}

float calcThickness(vec3 rayStart, vec3 rayDir)
{
    float thickness = 0.0;
    float stepSize = far / 128.0;

    for (float t = 0.0; t < far; t += stepSize) {
        vec3 pos = rayStart + rayDir * t;
        thickness += sampleDensity(pos) * stepSize;
    }

    return thickness;
}

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
    normal = normal * 0.5 + 0.5;

    vec4 surfacePos = cameraToView * vec4(positionInView.xyz + normal, 1);
    
    float depth = surfacePos.z / surfacePos.w;
    float z = depth * 2.0 - 1.0;
    float linearDepth = (2.0 * 1.0) / (20.0 + 1.0 - z * (20.0 - 1.0));
    gl_FragDepth = depth*0.5+0.5;

    out_color = vec4(vec3(linearDepth), 1);
}
