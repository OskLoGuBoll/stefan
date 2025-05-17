#version 430
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D grebuky;
uniform sampler2D screenDepth;

void main()
{
    float depth = texture(screenDepth, TexCoords).r;
    gl_FragDepth = depth;

    FragColor = texture(grebuky, TexCoords);
}
