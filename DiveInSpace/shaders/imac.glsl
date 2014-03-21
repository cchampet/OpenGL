#if defined(VERTEX)

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;
uniform float Time;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position;
out vec4 color;


void main(void)
{   
    uv = VertexTexCoord;

	
	normal = vec3(Object * vec4((VertexNormal+1.0)*0.5, 1.0));
	 
	// Rotation
	mat3 rotX = mat3(
		vec3(1, 0, 0),
		vec3(0, cos(Time), -sin(Time)),
		vec3(0, sin(Time), cos(Time))
		);
	mat3 rotY = mat3(
		vec3(cos(Time), 0, sin(Time)),
		vec3(0, 1, 0),
		vec3(-sin(Time), 0, cos(Time))
		);

	// Red : 0
	// Blue : 1
	// Green : 2

	// Scale
	if(gl_InstanceID == 1) {
		position = vec3(Object * vec4(VertexPosition*2, 1.0));
	} 
	else if(gl_InstanceID == 2) {
		position = vec3(Object * vec4(VertexPosition*1.2, 1.0));
	}
	else {
		position = vec3(Object * vec4(VertexPosition, 1.0));
	}

	// Position
    if(gl_InstanceID == 0) {
		position.x -= 1;
		position.y -= 1;
		position.z += 2;
		color = vec4(1.f, 0.f, 0.f, 1.f);
	}

	else if(gl_InstanceID == 2) {
		position.x += 1;
		position.y += 1;
		position.z += 3;
		color = vec4(0.f, 1.f, 0.f, 1.f);
	}

	else {
		color = vec4(0.f, 0.f, 1.f, 1.f);
	}
	
	// Rotation
	position *= rotX;
    position *= rotY;
   
    position.z += 2*Time;
    
    gl_Position = Projection * View * vec4(position, 1.0);

}

#endif

#if defined(FRAGMENT)

in vec2 uv;
in vec3 position;
in vec3 normal;
in vec4 color;

uniform sampler2D Diffuse;
uniform sampler2D Spec;

out vec4  Color;
out vec4  Normal;

void main ()
{

    vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = texture(Spec, uv).r;


	Color = color;//vec4(diffuse, spec);
	Normal = vec4(normal, spec);
}
#endif