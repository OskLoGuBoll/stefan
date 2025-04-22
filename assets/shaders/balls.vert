#version 150
in vec2 in_Position;
in vec2 in_TexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(in_Position, 0.0, 1.0);
    TexCoords = in_TexCoords;
}