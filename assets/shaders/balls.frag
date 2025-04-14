#version 150

in vec4 color;
in float radius;

out vec4 out_color;

void main(void)
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    // Soft edge with anti-aliasing
    float alpha = smoothstep(0.5, 0.45, dist);
    
    // Discard fully transparent fragments
    if (alpha <= 0.0) discard;
    
    // Optional: Add simple lighting based on position in the point sprite
    float light = 0.5 + 0.5 * dot(normalize(vec3(coord, sqrt(1.0 - dot(coord, coord)))), 
                       normalize(vec3(0.5, 0.5, 1.0)));
    
    out_color = vec4(color.rgb * light, color.a * alpha);
}
