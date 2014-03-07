#if defined(VERTEX)

in vec2 VertexPosition;

out vec2 uv;

void main(void)
{   
    uv = VertexPosition * 0.5 + 0.5;
    gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}

#endif

#if defined(FRAGMENT)

in vec2 uv;

uniform sampler2D Texture1;
uniform int SampleCount;
uniform vec2 Direction;
uniform vec2 TexelSize;

out vec4  OutColor;

void main ()
{
	mat3 I;
	vec3 average = vec3(0);
    for (float i = 0; i < SampleCount/2.0; i++) {
	    for (float j = 0; j < SampleCount/2.0; j++) {
	    	// get color of fragment in Texture1
	        vec3 sample  = texelFetch(Texture1, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
	        average += sample; 
	    }
	}

	average /= SampleCount;

    OutColor = vec4(average, 1.0);
}
#endif