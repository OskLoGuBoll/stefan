#version 430

in vec2 ex_BufferCoord;
in vec2 ex_Position;

out vec4 FragColor;

uniform sampler2D colorBuffer;
uniform sampler2D screenDepth;
uniform sampler2D depthMap;

uniform mat4 modelToWorld;
uniform mat4 worldToCamera;
uniform mat4 cameraToView;

uniform float kValue = 1;
uniform vec3 lightDir1 = vec3(1,0,1);
vec3 lightPos = vec3(200,10,0);

vec3 reconstructWorldPosition(vec2 ndc, float depth)
{
    // Convert to clip space
    vec4 clipSpacePos = vec4(ndc, depth, 1.0);

    // Camera space
    vec4 viewSpacePos = inverse(cameraToView) * clipSpacePos;
    viewSpacePos /= viewSpacePos.w;

    return viewSpacePos.xyz;
}

vec3 Color(vec3 k, float thickness)
{
    return exp(-k * thickness);
}

void main()
{
    float thickness = texture(depthMap, ex_BufferCoord).g;
    vec3 absorption = vec3(0.9, 0.6, 0.1);
    vec3 color = Color(absorption, thickness);

    float depth = texture(screenDepth, ex_BufferCoord).r;
    gl_FragDepth = depth;
    vec3 normal = texture(colorBuffer, ex_BufferCoord).rgb;
    normal = normalize(normal*2.0-1.0);
    float blurredDepth = texture(depthMap, ex_BufferCoord).r;

    vec3 reconstructedRay = reconstructWorldPosition(ex_Position,depth);

    // Compute lighting
    vec3 newlightPos = ((worldToCamera) * vec4(lightPos,1)).xyz;
    vec3 lightDir2 = normalize(newlightPos - reconstructedRay);
    vec3 viewDir  = normalize(-reconstructedRay);
    vec3 reflectDir = reflect(-lightDir2, normal);

    // Phong shading
    float diff = max(dot(normal, lightDir2), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    vec3 ambient = 0.1 * color;
    vec3 diffuse = diff * color;
    vec3 specular = spec * color;

    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor, thickness);
}
