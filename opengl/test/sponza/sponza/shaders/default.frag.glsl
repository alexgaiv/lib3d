#version 130

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;
#ifdef NORMALMAP  
in vec3 fTangent;
in vec3 fBinormal;
#endif

uniform mat4 View;

uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform sampler2D OpacityMask;

const int MM_NO_LIGHTING = 0;
const int MM_LAMBERT = 1;
const int MM_BLINN_PHONG = 2;

uniform struct
{
	vec4 diffuse;
	vec4 specular;
	float shininess;
	int useDiffuseMap;
	int useSpecularMap;
	int useNormalMap;
	int useOpacityMask;
	int mode;
} Material;

struct Light
{
	vec4 position;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	float radius;
};

const int MAX_LIGHTS = 9;
//uniform int NumLights;
//uniform Light Lights[MAX_LIGHTS];

//*
const Light lightSources[MAX_LIGHTS] = {
	Light(
		vec4(1.0, 1.0, -1.0, 0.0),
		vec4(0.2, 0.2, 0.2, 1.0),
		//vec4(0.15, 0.15, 0.15, 1.0),
		vec4(0.05),
		vec4(0.2),
		0),
	Light(
		vec4(-9.0, 66.0, 107.0, 1.0),
		vec4(1.0, 0.0, 0.0, 1.0),
		vec4(1.0, 0.0, 0.0, 1.0),
		vec4(1.0),
		150.0),
	Light(
		vec4(-13.0, 62.0, -126.0, 1.0),
		vec4(1.0, 0.0, 0.0, 1.0),
		vec4(1.0, 0.0, 0.0, 1.0),
		vec4(1.0),
		150.0),
	Light(
		vec4(-282.0, 68.0, -7.0, 1.0),
		vec4(0.0, 0.0, 1.0, 1.0),
		vec4(0.0, 0.0, 1.0, 1.0),
		vec4(1.0),
		150.0),
	Light(
		vec4(-240.0, 24.0, 80.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0),
		150.0),
	Light(
		vec4(-241.0, 21.0, -91.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0),
		150.0),
	Light(
		vec4(243.0, 68.0, -7.0, 1.0),
		vec4(0.0, 0.0, 1.0, 1.0),
		vec4(0.0, 0.0, 1.0, 1.0),
		vec4(1.0),
		150.0),
	Light(
		vec4(240.0, 24.0, 80.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0),
		150.0),
	Light(
		vec4(241.0, 21.0, -91.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0, 0.65, 0.0, 1.0),
		vec4(1.0),
		150.0),
};
//*/

vec3 fragNormal;
vec4 mtl_diffuse;

float GetDiffuse(in vec3 lightDir)
{
	return max(0.0, dot(fragNormal, lightDir));
}

#ifdef SPECULAR
float GetSpecular(in vec3 lightDir)
{
	vec3 viewDir = normalize(-fPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specAngle = max(0.0, dot(fragNormal, halfDir));
	return pow(specAngle, Material.shininess);
}
#endif

vec4 PhongLight(in Light l)
{
	vec3 lightDir;
	if (l.position.w == 0.0)
		lightDir = vec3(View * l.position);
	else lightDir = vec3(View * l.position) - fPosition;

	float att;
	if (l.radius != 0) {
		float dist = length(lightDir);
		att = clamp(1.0 - dist / l.radius, 0.0, 1.0);
		att *= att;
	}
	else att = 1;

	lightDir = normalize(lightDir);
	
	vec4 color = (l.ambient + l.diffuse * GetDiffuse(lightDir)) * mtl_diffuse * att;

#ifdef SPECULAR
	vec4 mtl_specular = mix(Material.specular, texture(SpecularMap, fTexCoord), Material.useSpecularMap);
	color += l.specular * att * mtl_specular * GetSpecular(lightDir);
#endif
	return color;
}

void main()
{
#ifdef OPACITYMASK
	vec4 c = texture(OpacityMask, fTexCoord);
	if (c.r < 0.5) discard;
#endif

	mtl_diffuse = mix(Material.diffuse, texture(ColorMap, fTexCoord), Material.useDiffuseMap);
	//mtl_diffuse = vec4(0.6);

	if (Material.mode == MM_NO_LIGHTING) {
		gl_FragColor = mtl_diffuse;
		return;
	}

#ifdef NORMALMAP
	vec3 t = vec3(texture(NormalMap, fTexCoord));
	mat3 tbn = mat3(normalize(fTangent), normalize(fBinormal), normalize(fNormal));
	t.y = 1.0 - t.y;
	fragNormal = normalize(tbn * (t * 2.0 - vec3(1.0)));
#else
	fragNormal = normalize(fNormal);
#endif

	//gl_FragColor = PhongLight(lightSources[0]);
	gl_FragColor = vec4(0.0);
	for (int i = 0; i < MAX_LIGHTS; i++)
	{
		//if (i >= NumLights) break;
		//gl_FragColor += PhongLight(Lights[i]);
		gl_FragColor += PhongLight(lightSources[i]);
	}
}