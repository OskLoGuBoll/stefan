#version 430

in vec2 ex_BufferCoord;

out vec4 FragColor;

uniform sampler2D colorBuffer;
uniform sampler2D screenDepth;

uniform mat4 worldToCamera;

uniform float kValue = 1;
uniform vec3 lightDir = vec3(1, 0, 1);

void main()
{
    float depth = texture(screenDepth, ex_BufferCoord).r;
    gl_FragDepth = depth;

    vec3 normal = texture(colorBuffer, ex_BufferCoord).rgb;

    vec3 color = vec3(0.1, 0.5, 0.7);

    float diffuse = kValue * max(0, dot(normal, lightDir));

    FragColor = vec4(color * diffuse, 1);
}
