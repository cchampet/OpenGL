#if defined(VERTEX)

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;
uniform float Time;
uniform vec3 CameraPosition;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{	
	/**
	* Rotation on himself
	*/
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
	/**
	* Rotation around the world
	*/
	float rotationX = cos(Time);
	float rotationY = sin(Time);

	/**
	* Compute uv, normal, and position of each vertices.
	*/
	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0));; 

	position = vec3(VertexPosition); //+ vec3(gl_InstanceID*rotationX, gl_InstanceID*rotationY, 0));
	//rotation on himself
	position *= rotX;
	position *= rotY;
	//offset between elements
	position.x += (gl_InstanceID % 50) * 2; 
	position.y += (int(gl_InstanceID/50)) * 2;
	//movement of elements
	position.z += cos(gl_InstanceID/15.)*10*cos(Time);
	//depend on the camera
	//position.x += CameraPosition.x;

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
