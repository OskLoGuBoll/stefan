#version 430
in vec2 in_Position;
in vec2 in_TexCoords;

out vec2 ex_BufferCoord;
out vec2 ex_Position;

void main()
{
    gl_Position = vec4(in_Position, 0.0, 1.0);
    ex_BufferCoord = in_TexCoords;
    ex_Position = in_Position;
}
