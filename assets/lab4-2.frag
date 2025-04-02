#version 150

in vec3 ex_Normal;
in vec4 ex_Position;

out vec4 out_Color;

uniform mat4 worldToCamera;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];
uniform bool isDirectional[4];
uniform float specularStrength;
//uniform vec4 modelColor;

uniform sampler2D textureUnit;
in vec2 ex_TextureCoord;

vec3 lightDirection;
uniform vec3 kValues;

void main(void)
{
	vec4 shading = vec4(0,0,0,0);
	for (int i=3; i<4; ++i)
	{
		if (isDirectional[i])
		{	
			// If directional we just want to rotate the direction vector to get the
			// new direction in camera space. Not rotate it AND translate to camera space.
			lightDirection = normalize(mat3(worldToCamera) * lightSourcesDirPosArr[i]);

		} else {
			// If positional we need to move the light in the world. Then get the vector from 
			// the light source to the "object".
			vec4 lightPosInCamera = worldToCamera * vec4(lightSourcesDirPosArr[i], 1.0);
			lightDirection = normalize(lightPosInCamera.xyz - ex_Position.xyz); 
		}
		// Diffuse
		float shade = kValues.y*max(0,dot(normalize(ex_Normal), lightDirection));

		// Specular
		// Incident vector, I, as 0-lightDirection. Normal vector as N.
		vec3 r = reflect(-lightDirection, normalize(ex_Normal)); // Calculated as I - 2.0 * dot(N, I) * N.
		// Could also be calculated as this:
		//vec3 r = vec3(2,2,2)*normalize(ex_Normal)*dot(normalize(ex_Normal), lightDirection)-lightDirection;

		vec3 viewDirection = normalize(-ex_Position.xyz);
		shade += kValues.z*max(0,pow(dot(viewDirection,r),specularStrength));
		shading = shading + vec4( shade, shade, shade, 1 ) * vec4(lightSourcesColorArr[i],1);
	}
	// Ambient
	shading += vec4(kValues.x,kValues.x,kValues.x,0);
	out_Color = texture(textureUnit, ex_TextureCoord) * shading;
	//out_Color = vec4(ex_Normal,0);
}
