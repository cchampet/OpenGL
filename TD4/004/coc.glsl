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

uniform sampler2D Depth;
uniform mat4 ScreenToView;
uniform vec3 Focus;
uniform float Near;
uniform float Far;
uniform float FocusPlane;

out vec4  Color;

void main(void)
{
	// Camera depth
	float depth = texture(Depth, uv).r;
	vec2  xy = uv * 2.0 -1.0;
	vec4  wViewPos =  ScreenToView * vec4(xy, depth * 2.0 -1.0, 1.0);
	vec3  viewPos = vec3(wViewPos/wViewPos.w);
	float viewDepth = -viewPos.z;


	float blurCoeff = 0;
	if(viewDepth < FocusPlane) {
        blurCoeff = abs((viewDepth - FocusPlane)/Near);
        //Color = vec4(1.0, 0.0, 0.0, 1.0);

	}
	else {
        blurCoeff = abs((viewDepth - FocusPlane)/Far);
        //Color = vec4(0.0, 0.0, 1.0, 1.0);

	}

	vec4 color = vec4(0.0, 0.0, 1.0, 0.0);
	if(blurCoeff >= 0) {
		color.r = 1.0;
	}
	else if(blurCoeff < 1.0) {
		color.r = 1.0;
	}


    Color = vec4(blurCoeff, 0.0, 0.0, 1.0);
}

#endif
