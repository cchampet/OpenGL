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

mat3 rotX = mat3(
	vec3(1, 0, 0),
	vec3(0, cos(Time), -sin(Time)),
	vec3(0, sin(Time), cos(Time))
	);

float rotationSpeed = 10.;
mat3 rotY = mat3(
	vec3(cos(Time/rotationSpeed), 0, sin(Time/rotationSpeed)),
	vec3(0, 1, 0),
	vec3(-sin(Time/rotationSpeed), 0, cos(Time/rotationSpeed))
	);

void main(void)
{	
	uv = VertexTexCoord;
	normal = vec3(Object * vec4((VertexNormal+1.0)*0.5, 1.0));
	position = vec3(VertexPosition);
	//scale element
	position *= vec3(10, 10, 10);
	//rotation on himself
	position *= rotY;
	//offset
	position.x -= 5;
	position.y += 5;

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
