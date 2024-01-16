#version 420 core

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D uTexture;

uniform vec3 viewPos;

uniform DirLight dirlight;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	vec3 ambient = light.ambient;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse;

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 16.0); // change to material shininess
	vec3 specular = 0.5 * spec * light.specular;

	return (ambient + diffuse + specular);
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0);

	result += calcDirLight(dirlight, norm, viewDir);

	result *= texture(uTexture, TexCoord).xyz;// vec3(0.388, 0.851, 0.035);

	FragColor = vec4(result, 1.0);
}