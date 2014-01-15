uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

uniform float Time;

#if defined(VERTEX)

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
}vertex;

void main(void)
{	
	
	gl_Position = vec4(VertexPosition + vec3(gl_InstanceID, gl_InstanceID, 0), 1.0);

	vertex.uv = VertexTexCoord;
	vertex.normal = VertexNormal;
	vertex.position = VertexPosition;
}

#endif

#if defined(GEOMETRY)

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform int Animation;

in vData
{
	vec2 uv;
    vec3 normal;
    vec3 position;

}vertices[];

out fData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
}frag;    

void main()
{
	int i;
	for (i = 0; i < gl_in.length(); ++i)
	{
		if(Animation == 0) {
			gl_Position =  Projection * View * gl_in[i].gl_Position;
		}
		else {
			gl_Position =  Projection * View * vec4(gl_in[i].gl_Position.x + gl_PrimitiveIDIn%2*cos(Time), gl_in[i].gl_Position.y + gl_PrimitiveIDIn%2*sin(Time), gl_in[i].gl_Position.z, 1.0);
		}
		frag.normal = vertices[i].normal;
		frag.position = vertices[i].position;
		frag.uv = vertices[i].uv;
		EmitVertex();
	}
	EndPrimitive();
}

#endif

#if defined(FRAGMENT)
uniform vec3 CameraPosition;

in fData
{
	vec2 uv;
    vec3 normal;
    vec3 position;
}frag;

out vec4 Color;

void main(void)
{
	//ColorOut = vec4(1.0, 0.0, 1.0, 1.0);
	//Color = vec4(frag.position.x, frag.position.y, frag.position.z, 1.0);
	//Color = vec4(frag.normal.x, frag.normal.y, frag.normal.z, 1.0);
	int tmpCoordX = int(frag.uv.x * 10)+1;
	int tmpCoordY = int(frag.uv.y * 10)+1;
	Color = vec4((tmpCoordX%2)*(tmpCoordY%2)*sin(5*frag.uv.y), (tmpCoordX%2), 0.5, 1.0);
	//float x = uv.x + sin(Time);
	//float y = uv.y + cos(Time);

	//x = x*50 - 20;
	//y = y*50 - 20 ;
	//y*= 3.14159;

	//if(( int(float(abs(x-sin(y)))%10 <4))) {
	//	color = vec4(1.0*x, 0.25*sin(Time), 0.1*cos(Time), 1.0);;
	//}

	//Color = color;
}	
#endif
