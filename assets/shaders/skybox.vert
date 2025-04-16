#version 150

in  vec3 in_Position;

out vec3 ex_TextureCoord;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;


void main(void)
{
	gl_Position = cameraToView * worldToCamera * modelToWorld * vec4(in_Position, 1.0);
	//gl_Position = modelToWorld * vec4(in_Position, 1.0);
	ex_TextureCoord = in_Position;	
}
