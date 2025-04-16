#version 150

in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D screenTexture;

void main()
{
    fragColor = vec4(texture(screenTexture, texCoord).rgb, 1.0);
}

