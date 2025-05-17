#version 430

in vec3 ex_TextureCoord;
out vec4 out_Color;

uniform samplerCube textureUnitCube;


void main(void)
{
	out_Color = texture(textureUnitCube, -ex_TextureCoord);
}
