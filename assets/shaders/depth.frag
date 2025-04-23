#version 150

in vec4 positionInView;
in float distanceToCam;

out vec4 out_color;

uniform float near = 1;
uniform float far = 10;

void main(void)
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Soft edge with anti-aliasing
    float alpha = smoothstep(0.5, 0.45, dist);

    // Discard fully transparent fragments
    if (alpha <= 0.0) discard;

    // If not linear depth
    float depth = gl_FragCoord.z;

    // If linear depth
    //float linearDepth = length(positionInView.xyz); // eye-space depth
    float normalizedLinearDepth = (distanceToCam - near) / (far - near);

    out_color = vec4(vec3(normalizedLinearDepth), 1);
}
