#if defined(VERTEX)

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;
uniform float Time;

uniform float TranslateFactor;
uniform bool IsMonolitheStop;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{	
	/**
	* Compute uv, normal, and position of each vertices.
	*/
	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0));

	position = vec3(VertexPosition); 
	//scale elements
	position.x *= 10;
	position.y *= 15;
	position.z *= 2;
	if(!IsMonolitheStop){
		//offset of the element
		position.z -= 2000;//5000;
		//movement
		position.z += 25*TranslateFactor*Time;
	}

	gl_Position = Projection * View * vec4(position, 1.0);
}

#endif

#if defined(FRAGMENT)

uniform vec3 CameraPosition;
uniform float Time;

in vec2 uv;
in vec3 position;
in vec3 normal;

uniform sampler2D Diffuse;
uniform sampler2D Spec;

out vec4  Color;
out vec4  Normal;

void main(void)
{
	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;
	Color = vec4(diffuse, spec);
	Normal = vec4(normal, spec);
}

#endif
