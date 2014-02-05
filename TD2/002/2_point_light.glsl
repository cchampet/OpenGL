#if defined(VERTEX)

in vec2 VertexPosition;

out vec2 uv;

void main(void)
{	
	uv = VertexPosition * 0.5 + 0.5; // Astuce : on retrouve les normales à partir des positions des vertices
	gl_Position = vec4(VertexPosition.xy, 0.0, 1.0);
}

#endif

#if defined(FRAGMENT)

in vec2 uv;

uniform sampler2D Material;
uniform sampler2D Normal;
uniform sampler2D Depth;

uniform vec3 CameraPosition;

uniform vec3  LightPosition;
uniform vec3  LightColor;
uniform float LightIntensity;
uniform float LightSpecCoeff;

uniform mat4 InverseViewProjection;

out vec4  Color;

vec3 computePointLight(float intensity, float specCoeff, vec3 diffuse, vec3 lPosition, vec3 position, vec3 normal, vec4 material) {
	
	vec3 n = normalize(normal);

	vec3  lightColor = LightColor;
	vec3  lightPosition = lPosition;

	// Vecteur surface vers lumière
	vec3 l =  lightPosition - position;

	vec3 v = position - CameraPosition; // Vecteur surface vers caméra
	vec3 h = normalize(l-v); // Normale
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);

	float distance = sqrt(dot(l, l));
	float coeffIntensity = 1.0;
	float epsilon = 0.001;
	if(distance - epsilon > 0.f){
		coeffIntensity = 1./(pow(distance, 2.f));
	}
	float lightIntensity = intensity*coeffIntensity;

	vec3 colorRes = lightColor * lightIntensity * (diffuse * n_dot_l + specCoeff * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, specCoeff * LightSpecCoeff));
	return colorRes;
}

void main(void)
{
	vec3 normal = texture(Normal, uv).xyz;
	vec4 material = texture(Material, uv);
	vec3 diffuse = material.xyz;
	float specularCoeff = material.z;
	float depth = texture(Depth, uv).x;

	vec2  xy = uv * 2.0 -1.0;
	vec4  wPosition =  vec4(xy, depth * 2.0 -1.0, 1.0) * InverseViewProjection;
	vec3  position = vec3(wPosition/wPosition.w);

	vec3 light = computePointLight(LightIntensity, specularCoeff, diffuse, LightPosition, position, normal, material);
	Color = vec4(light, 1.);
}

#endif
