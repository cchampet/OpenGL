#if defined(VERTEX)

in vec2 VertexPosition;
in vec2 VertexTexCoord;

out vec2 uv;

void main(void)
{   
	uv = VertexPosition * 0.5 + 0.5;
    gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}

#endif

#if defined(FRAGMENT)

uniform sampler2D Texture1;
uniform float Gamma;

in vec2 uv;

out vec4  OutColor;

void main(void)
{
	vec3 diffuse = texture(Texture1, uv).rgb;
	OutColor = vec4(pow(diffuse, vec3(1/Gamma)), 1.0);
}

#endif
