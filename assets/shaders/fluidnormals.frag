#version 430

in vec2 ex_BufferCoord;

out vec4 NormalColor;

uniform sampler2D colorBuffer;
uniform sampler2D screenDepth;
uniform float maxdepth = 1;

uniform mat4 cameraToView;
uniform vec2 texelSize;

vec3 uvToPosition(vec2 uv, float depth)
{
    float x = uv.x * 2.0 - 1.0;
    float y = uv.y * 2.0 - 1.0;
	float z = depth * 2.0 - 1.0;

    vec4 ndc = vec4(x, y, z, 1.0);
    vec4 eye = inverse(cameraToView) * ndc;
    return eye.xyz / eye.w;
}

vec3 getPosition(vec2 uv)
{
    float depth = texture(colorBuffer, uv).r;
    return uvToPosition(uv, depth);
}

void main()
{
    float depth = texture(colorBuffer, ex_BufferCoord).r;
    if (depth > maxdepth)
    {
        discard;
        return;
    }

    vec3 pos = uvToPosition(ex_BufferCoord, depth);

    vec3 ddx = getPosition(ex_BufferCoord + vec2(texelSize.x, 0)) - pos;
    vec3 ddx2 = pos - getPosition(ex_BufferCoord + vec2(-texelSize.x, 0));
    if (abs(ddx.z) > abs(ddx2.z)) {
         ddx = ddx2;
    }

    vec3 ddy = getPosition(ex_BufferCoord + vec2(0, texelSize.y)) - pos;
    vec3 ddy2 = pos - getPosition(ex_BufferCoord + vec2(0, -texelSize.y));
    if (abs(ddy.z) > abs(ddy2.z)) {
         ddy = ddy2;
    }

    vec3 normal = normalize(cross(ddx, ddy));

    NormalColor = vec4(normal*0.5+0.5, 1);

	gl_FragDepth = texture(screenDepth, ex_BufferCoord).r;
}
