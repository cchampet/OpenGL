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

uniform sampler2D Color;
uniform sampler2D Blur;
uniform sampler2D CoC;


out vec4  OutColor;

void main(void)
{
    OutColor = vec4(1.0, 0.0, 1.0, 1.0);
}

#endif
