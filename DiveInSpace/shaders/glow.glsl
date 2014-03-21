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
	
	//more the scene has colors, more we display the scene
	//less the scene has colors, more we display the blur
	
	//curious mix !
	//vec3 mix = vec3(renderScene.r/blur.r, renderScene.g/blur.g, renderScene.b/blur.b);
	//OutColor = vec4(mix, 1.0);
	
	//the mix in theory
	//float mix = (renderScene.r + renderScene.y + renderScene.z) / 3.;
	//OutColor = vec4((renderScene*mix)+(blur*(1-mix)), 1.0);
	
	OutColor = vec4((renderScene+(blur*0.2))*0.5, 1.0);
	if(renderScene == vec3(0, 0, 0)){
		//full blur
		OutColor = vec4(blur, 1.0);
	}
}

#endif
