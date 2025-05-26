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

uniform vec4 kValues = vec4(1, 1, 1,100);
vec3 lightPos = vec3(200,200,0);

vec3 reconstructWorldPosition(vec2 ndc, float depth)
{
    vec4 screenSpacePos = vec4(ndc, depth, 1.0);

    vec4 cameraSpacePos = inverse(cameraToView) * screenSpacePos;
    cameraSpacePos /= cameraSpacePos.w;

    return cameraSpacePos.xyz;
}

vec3 color(vec3 k, float thickness)
{
    return exp(-k * thickness);
}

void main()
{
    float thickness = texture(depthMap, ex_BufferCoord).g;
    vec3 absorption = vec3(0.9, 0.6, 0.1);
    vec3 color = color(absorption, thickness);

    float depth = texture(screenDepth, ex_BufferCoord).r;
    gl_FragDepth = depth;
    vec3 normal = texture(colorBuffer, ex_BufferCoord).rgb;
    normal = normalize(normal*2.0-1.0);
    float blurredDepth = texture(depthMap, ex_BufferCoord).r;

    vec3 reconstructedRay = reconstructWorldPosition(ex_Position,blurredDepth);

    // Compute lighting
    vec3 newlightPos = ((worldToCamera) * vec4(lightPos,1)).xyz;
    vec3 lightDir2 = normalize(newlightPos - reconstructedRay);
    vec3 viewDir  = normalize(-reconstructedRay);
    vec3 reflectDir = reflect(-lightDir2, normal);

    // Phong shading
    float diff = kValues.x * max(dot(normal, lightDir2), 0.0);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), kValues.w);

    vec3 ambient = kValues.x * color;
    vec3 diffuse = kValues.y * diff * color;
    vec3 specular = kValues.z * spec * color;

    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor, thickness);
}
