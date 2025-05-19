#version 430
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D grebuky;
uniform sampler2D screenDepth;

void main()
{
    float depth = texture(screenDepth, TexCoords).r;
    gl_FragDepth = depth;

    vec3 normal = texture(grebuky, TexCoords).rgb;
    
    float diffuse = max(0, dot(normal, vec3(1, 0, 1)));
    
    vec3 color = vec3(0, 0.5, 1);

    FragColor = vec4(color * diffuse, 1);
}
