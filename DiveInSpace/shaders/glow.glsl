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

uniform sampler2D TextureBlur;
uniform sampler2D TextureColor;

out vec4 OutColor;

void main(void)
{
	vec3 blur = texture(TextureBlur, uv).rgb;
	vec3 renderScene = texture(TextureColor, uv).rgb;
	
	//mix the color of the scene with the blur
	OutColor = vec4((renderScene+blur)/2., 1.0);
	if(renderScene == vec3(0, 0, 0)){
		//display full blur if the scene is black
		OutColor = vec4(blur, 1.0);
	}
}

#endif
