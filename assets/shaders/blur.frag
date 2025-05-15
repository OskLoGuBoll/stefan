#version 150

in vec2 ex_BufferCoord;
out vec4 FragColor;

uniform sampler2D screenDepth;
uniform sampler2D blurBuffer;

uniform vec2 u_Scale;

const vec2 gaussFilter[7] = vec2[] 
( 
	vec2(-3.0,	0.015625),
	vec2(-2.0,	0.09375),
	vec2(-1.0,	0.234375),
	vec2(0.0,	0.3125),
	vec2(1.0,	0.234375),
	vec2(2.0,	0.09375),
	vec2(3.0,	0.015625)
);

void main()
{

	vec4 color = vec4(0,0,0,0);
	for( int i = 0; i < 7; i++ )
        for( int j = 0; j < 7; j++ )
        {
            color += texture(blurBuffer, vec2( ex_BufferCoord.x+gaussFilter[i].x*u_Scale.x, ex_BufferCoord.y+gaussFilter[j].x*u_Scale.y ) )*gaussFilter[i].y*gaussFilter[j].y;
        }

	FragColor = color;

    float depth = texture(screenDepth, ex_BufferCoord).r;
    gl_FragDepth = depth;
}
