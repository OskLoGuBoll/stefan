#version 150

in vec2 inPosition;
in vec2 inTexCoord;

out vec2 texCoord;

void main()
{
    gl_Position = vec4(inPosition, 0, 1);
    texCoord = inTexCoord;
}

