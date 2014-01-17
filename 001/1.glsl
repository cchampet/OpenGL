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
	/* Texture procédurale */
	//x => les bandes
	//x-cos(y) => les vagues
	float x = frag.uv.x + sin(Time); // -1/2
	float y = frag.uv.y + cos(Time); // -1/2

	x = x*50 - 20; // -30/80
	y = y*50 - 20; // -30/80
	
	//y*= 3.14159;

	vec4 color = vec4(0.0, 0.0, 1.0, 1.0);

	//x - sin(y)
	if(int(abs(x-cos(y)))%10 < 6) {
		color = vec4(x, 0.25, 0.1, 1.0);
	}
	Color = color;

	/* Lumières */
	vec3 l = vec3(0.0, 0.3, 0.2); // vecteur surface vers lumière
	vec3 v = normalize(CameraPosition - frag.normal); // vecteur surface vers caméra
	vec3 n = frag.normal; // normale
	vec3 h = normalize(l-v);

	vec4 diffuse = color;
	vec4 specular = vec4(0.5, 0.5, 0.5, 1.0);
	float specular_power = 1.0;

	Color = diffuse * dot(n, l) + specular *  pow(dot(n, h), specular_power);
}

#endif