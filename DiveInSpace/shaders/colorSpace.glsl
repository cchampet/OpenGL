#define PI 3.14159265358979

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
 
uniform vec3 	Resolution;		// Viewport Resolution (in pixels)
uniform float 	Time;			// Shader playback time (in seconds)

out vec4 Color;

// Convert HSL colorspace to RGB. http://en.wikipedia.org/wiki/HSL_and_HSV
vec3 HSLtoRGB(in float h, in float s, in float l)
{
    vec3 rgb = clamp(abs(mod(h + vec3(0.0, 4.0, 2.0), 6.0)-3.0)-1.0, 0.0, 1.0 );
    return l + s * (rgb - 0.5)*(1.0 - abs(2.0*l - 1.0));
}

void main(void)
{
	vec2 toto = gl_FragCoord.xy;

	// Effet stylé rayures horizontales
	/*if(toto.x > 0.5) {
		toto.x = 0.8;
	}*/

	// Effet stylé 2
	toto.x = 700;


	//if(toto.x > 500) {
		
	//}

	//toto = vec2(500, 500);

	vec2 r = toto / Resolution.xy;

	//if(toto.x > 500) {
		//r.x = 0.5;
	//}
	vec2 p = -1.0 + 2.0*r;

	p.x *= Resolution.x/Resolution.y;
	float fSin = sin(Time*0.4);
	float fCos = cos(Time*0.4);
	p *= mat2(fCos, -fSin, fSin, fCos);

	float h = atan(p.x, p.y) + PI;
	float x = distance(p, vec2(0.0));
	float a = -(0.6 + 0.2*sin(Time*3.1 + sin((Time*0.8 + h*2.0)*3.0))*sin(Time + h));
	float b = -(0.8 + 0.3*sin(Time*1.7 + sin((Time + h*4.0))));
	float c = 1.25 + sin((Time + sin((Time + h)*3.0))*1.3)*0.15;
	float l = a*x*x + b*x + c;

	Color = vec4(HSLtoRGB(h*3.0/PI, 1.0, l),1.0);
}


#endif
