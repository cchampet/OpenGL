#if defined(VERTEX)
uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Object;

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
	gl_Position = Projection * View * Object * vec4(VertexPosition, 1.0);
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
//pointLight
uniform float Intensity1;
uniform float SpecCoeff1;
uniform vec3 LightColor1;
uniform vec3 LightPosition1;
//pointLight
uniform float Intensity2;
uniform float SpecCoeff2;
uniform vec3 LightColor2;
uniform vec3 LightPosition2;
//directionnalLight
uniform float Intensity3;
uniform float SpecCoeff3;
uniform vec3 LightColor3;
uniform vec3 LightPosition3;
//spotLight
uniform float Intensity4;
uniform float SpecCoeff4;
uniform vec3 LightColor4;
uniform vec3 LightPosition4;
uniform vec3 SpotDirection4;
uniform float SpotAngle4;

out vec4 Color;

// Type :
// 0 = PointLight 
// 1 = DirectionalLight
// 2 = SpotLight
vec3 computeColor(float intensity, float specCoeff, vec3 color, vec3 lPosition, int type) {
	
	vec3 diffuse = texture(Diffuse, uv).rgb;
	float spec = specCoeff;
	vec3 n = normalize(normal);

	vec3  lightColor = color;
	vec3  lightPosition = lPosition;
	//vec3  lightPosition = vec3(sin(Time) *  10.0, 1.0, cos(Time) * 10.0);

	vec3 l; // Vecteur surface vers lumière
	if(type == 0 || type == 2) {
		l =  lightPosition - position;
	}
	else if(type == 1) {
		l = -lPosition;
	}

	vec3 v = position - CameraPosition; // Vecteur surface vers caméra
	vec3 h = normalize(l-v); // Normale
	float n_dot_l = clamp(dot(n, l), 0, 1.0);
	float n_dot_h = clamp(dot(n, h), 0, 1.0);

	float fallOff = 1.f;
	if(type == 2) {
		// cosTheta = dot(a,b) / (norm(a)norm(b))
		float cosTheta = dot(l, SpotDirection4)/(sqrt(dot(SpotDirection4, SpotDirection4))*sqrt(dot(l, l)));
		float spotAngleExtern_rad = SpotAngle4*3.14/180;
		float spotAngleIntern_rad = 50.;
		if(cosTheta < cos(spotAngleExtern_rad)){
			fallOff = 0.f;
		}
		else if(cosTheta > cos(spotAngleExtern_rad) && cosTheta < cos(spotAngleIntern_rad)) {
			fallOff = pow(((cosTheta - cos(spotAngleExtern_rad)) / (cos(spotAngleIntern_rad) - cos(spotAngleExtern_rad))),4);
		}
		else{
			fallOff = 1.f;
		}
	}
	float distance = sqrt(dot(l, l));
	float coeffIntensity = 1./(pow(distance, 2.f));
	float lightIntensity = intensity*coeffIntensity;

	vec3 colorRes = lightColor * lightIntensity * fallOff * (diffuse * n_dot_l + spec * vec3(1.0, 1.0, 1.0) *  pow(n_dot_h, spec * 100.0));
	return colorRes;
}

void main(void)
{
	vec3 color1 = computeColor(Intensity1, SpecCoeff1, LightColor1, LightPosition1, 0);
	vec3 color2 = computeColor(Intensity2, SpecCoeff2, LightColor2, LightPosition2, 0);
	vec3 color3 = computeColor(Intensity3, SpecCoeff3, LightColor3, LightPosition3, 1);
	vec3 color4 = computeColor(Intensity4, SpecCoeff4, LightColor4, LightPosition4, 2);
	
	Color = vec4(color1+ color4, 1.0);
}
#endif
