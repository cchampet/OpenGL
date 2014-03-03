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
    for (int i=0; i<3; i++) {
	    for (int j=0; j<3; j++) {
	    	// On récupère la couleur d'un pixel précis dans Texture1
	        vec3 sample  = texelFetch(Texture1, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
	        average += sample; 
	    }
	}

	average /= 9;

    OutColor = vec4(average, 1.0);
}
#endif

