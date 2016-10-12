#version 130

in vec3 Vertex;
in vec3 Normal;
in vec2 TexCoord;
in vec3 Tangent;
in vec3 Binormal;

out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoord;
out vec3 fTangent;
out vec3 fBinormal;

uniform mat4 ModelView;
uniform mat4 NormalMatrix;
uniform mat4 ModelViewProjection;

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

void main()
{
	fPosition = (ModelView * vec4(Vertex, 1.0)).xyz;
	fTexCoord = TexCoord;
	fTexCoord.y = 1.0 - fTexCoord.y;

	fNormal = normalize((NormalMatrix * vec4(Normal, 1.0)).xyz);
	if (Material.useNormalMap) {
		fTangent = normalize((NormalMatrix * vec4(Tangent, 1.0)).xyz);
		fBinormal = normalize((NormalMatrix * vec4(Binormal, 1.0)).xyz);
	}
	else fTangent = fBinormal = vec3(0.0);

	gl_Position = ModelViewProjection * vec4(Vertex, 1.0);
}