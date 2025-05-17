#version 430

in vec3 in_Position;
in vec3 in_Normal;
in vec2 in_TextureCoord;

out vec3 ex_Normal;
out vec4 ex_Position;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

out vec2 ex_TextureCoord;

void main(void)
{
	gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(in_Position, 1.0);
	
	mat3 normalTransform = transpose(inverse(mat3(worldToCamera)*mat3(modelToWorld)));
	ex_Normal = normalTransform * in_Normal; // Normal vectors

	//ex_Light = mat3(worldToCamera)*normalize(light); // Light direction. In fragment shader

	ex_Position = worldToCamera * modelToWorld * vec4(in_Position, 1.0); // Surface position
	ex_TextureCoord = in_TextureCoord;
}
