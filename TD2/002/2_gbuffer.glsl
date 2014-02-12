#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

uniform float Time;

uniform float NbCubes;
uniform float CubesPerCircle;
uniform float SpaceBetweenCircle;
uniform float CubesRotation;

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;

out vec2 uv;
out vec3 normal;
out vec3 position;

void main(void)
{	
	uv = VertexTexCoord;
	normal = vec3(Object * vec4(VertexNormal, 1.0));; 
	position = vec3(Object * vec4(VertexPosition, 1.0));
	
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

	//the cubes
	if(gl_InstanceID > 0){

		//rotation on place
		if(CubesRotation > 0){
			position *= rotX;
			position *= rotY;
		}

		float varX = cos(gl_InstanceID + Time);
		float varZ = sin(gl_InstanceID + Time);
		//OpenGL Error because of (gl_InstanceID / CubesPerCircle) => int / float
		float ray = SpaceBetweenCircle * (1 + gl_InstanceID / CubesPerCircle);
		position.x += ray * varX;
		position.z += ray * varZ;

		float varY = cos(Time);
		if(gl_InstanceID%2 == 0)
			position.y += varY;
		else
			position.y -= varY;
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
	//display normal
	/*
	Normal = vec4(normal, 1.);
	Color = Normal;
	*/
	
	//display diffuse texture
	/*
	vec3 diffuse = texture(Diffuse, uv).rgb;
	Color = vec4(diffuse, 1.0);
	*/
	
	//display Specular texture (all red, it's just a coef !)
	/*
	vec3 spec = texture(Spec, uv).rgb;
	Color = vec4(spec, 1.0);
	*/

	//display diffuse + spec coef
	vec3 diffuse = texture(Diffuse, uv).rgb;
	vec3 spec = texture(Spec, uv).rgb;
	Color = vec4(diffuse, spec.x);
	Normal = vec4(normal, 1.);
}

#endif
