#version 150

in float radius;
in vec4 color;

out vec4 out_color;

void main(void)
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Soft edge with anti-aliasing
    float alpha = smoothstep(0.5, 0.45, dist);

    // Discard fully transparent fragments
    if (alpha <= 0.0) discard;

    out_color = vec4(color.xyz, color.a * alpha);
}
