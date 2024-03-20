#version 430 core

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	sampler2DArray shadow_map;
	mat4 shadow_view;
};

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D ssao;

uniform float cascaded_depths[4];
const float shadow_bias = 0.05;

float calcDirLightInShadow(DirLight light, vec3 pos)
{
	vec4 light_coords = light.shadow_view * vec4(pos, 1.0);
	float cascade_level = 0;
	//for (int i = 0; i < 3; ++i)
		//cascade_level = pos.z > cascaded_depths[i] ? i : cascade_level;
	float light_depth = texture(light.shadow_map, vec3(light_coords.xy, cascade_level)).x;
	return light_depth < light_coords.z + shadow_bias ? 1.0 : 0.0;
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, float ssao)
{
	vec3 lightDir = normalize(-light.direction);

	vec3 ambient = light.ambient * ssao;

	float in_light = 1.0;// -calcDirLightInShadow(light, frag_pos);

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse * vec3(in_light);

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32); // change to material shininess
	vec3 specular = 0.5 * spec * light.specular * vec3(in_light);

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

	FragColor = albedo_spec.w == 1.0 ? vec4(0.3, 0.7, 1.0, 1.0f) : vec4(result, 1.0);
}