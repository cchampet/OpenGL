#if defined(VERTEX)

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;
uniform float Time;

uniform float TranslateFactor;

uniform float SpiralRadius;
uniform float SpiralAngle;


in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{	
	float angle = SpiralAngle * 3.1415926 * gl_InstanceID / 50;

	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0)); 
	position = vec3(VertexPosition);
	//spirale
	if(gl_InstanceID % 2 == 0){
		position.x += pow(SpiralRadius, angle) * cos(angle);
		position.y += pow(SpiralRadius, angle) * sin(angle);
	}
	else{
		position.x += pow(SpiralRadius, angle) * sin(angle);
		position.y += pow(SpiralRadius, angle) * cos(angle);
	}
	//position.z -= 3000;//offset to have the time to start the video record
	position.z -= gl_InstanceID;
	//movement of elements
	position.z += 25*TranslateFactor*Time;
	gl_Position = Projection * View * vec4(position, 1.0);


	/*
	// Rotation on himself
	mat3 rotX = mat3(
		vec3(1, 0, 0),
		vec3(0, cos(Time), -sin(Time)),
		vec3(0, sin(Time), cos(Time))
		);

	// Compute uv, normal, and position of each vertices.
	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0)); 

	position = vec3(VertexPosition); 
	//scale elements
	position.x *= 0.2;
	position.y *= 0.2;
	position.z *= 0.2;
	//rotation elements on themself
	if(gl_InstanceID % 2 == 0)
		position *= rotX;
	else
		position *= inverse(rotX);
	//rotate elements between them
	position.x += cos(gl_InstanceID/10.) * 10;
	position.z += cos(gl_InstanceID/20.) * 10;
	if(gl_InstanceID % 2 == 0)
		position.y += cos(gl_InstanceID/10.) * cos(Time);
	else
		position.y += cos(gl_InstanceID/10.) * sin(Time);
	//offset between elements
	position.x += (gl_InstanceID % 20);
	position.y += (int(gl_InstanceID/50));
	//movement
	position.z += TranslateFactor*Time;
	*/

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
