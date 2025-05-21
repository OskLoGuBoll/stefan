#version 430

in vec2 ex_BufferCoord;
out vec4 FragColor;

uniform sampler2D colorBuffer;
uniform sampler2D screenDepth;
uniform vec2 u_Scale;
uniform float near;
uniform float far;

const vec2 gaussFilter[7] = vec2[]
( 
    vec2(-3.0, 0.015625),
    vec2(-2.0, 0.09375),
    vec2(-1.0, 0.234375),
    vec2( 0.0, 0.3125),
    vec2( 1.0, 0.234375),
    vec2( 2.0, 0.09375),
    vec2( 3.0, 0.015625)
);

float getLinearDepth(vec2 uv) {
    float d = texture(screenDepth, uv).r;
	float z = d * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
    // vec4 centerColor = texture(blurBuffer, ex_BufferCoord); // Used for full bilateral blur
    float centerDepth = getLinearDepth(ex_BufferCoord);
	float depthParam = centerDepth * 0.05; // Should be uniform

    vec4 result = vec4(0.0);
    float totalWeight = 0.0;

    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 7; ++j) 
        {
            vec2 offset = vec2(gaussFilter[i].x, gaussFilter[j].x) * u_Scale*7;
            vec2 sampleCoords = ex_BufferCoord + offset;

            vec4 sampleColor = texture(colorBuffer, sampleCoords);
            float spatialWeight = gaussFilter[i].y * gaussFilter[j].y;

            float sampleDepth = getLinearDepth(sampleCoords);
            float depthWeight = exp(-pow(sampleDepth - centerDepth,2) / (2.0 * pow(depthParam,2)));

            float weight = spatialWeight * depthWeight;

            result += sampleColor * weight;
            totalWeight += weight;
        }
    }

    result /= totalWeight;

    FragColor = result;

	gl_FragDepth = texture(screenDepth, ex_BufferCoord).r;
}
