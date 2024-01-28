#version 430 core

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float ssao)
{
	vec3 lightDir = normalize(-light.direction);

	vec3 ambient = light.ambient * ssao;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse;

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32); // change to material shininess
	vec3 specular = 0.5 * spec * light.specular;

	return (ambient + diffuse + specular);
}

uniform DirLight dirlight;

void main()
{
	vec3 FragPos = texture(gPosition, TexCoord).xyz;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec4 albedo_spec = texture(gAlbedoSpec, TexCoord);
	float ssao = texture(ssao, TexCoord).r;
	vec3 viewDir = normalize(-FragPos);

	vec3 result = vec3(0.0, 0.0, 0.0);

	result += calcDirLight(dirlight, normal, viewDir, ssao);
	
	result *= albedo_spec.xyz;

	FragColor = albedo_spec.w == 1.0 ? vec4(1.0, 4.0, 10.0, 1.0f) : vec4(result, 1.0);
}