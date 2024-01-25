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

uniform sampler2D ssaoNoise;

uniform vec3 ssao_samples[64];
uniform mat4 projection;
uniform mat4 view;

const float ssao_radius = 1.0;
const float ssao_bias = 0.025;

const vec2 noiseScale = vec2(900.0 / 4.0, 600.0 / 4.0);

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	vec3 ambient = light.ambient;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse;

	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32); // change to material shininess
	vec3 specular = 0.5 * spec * light.specular;

	return (ambient + diffuse + specular);
}

uniform DirLight dirlight;

uniform vec3 viewPos;

void main()
{
	vec3 FragPos = texture(gPosition, TexCoord).xyz;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec4 albedo_spec = texture(gAlbedoSpec, TexCoord);
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 ssao_noise_vec = texture(ssaoNoise, TexCoord * noiseScale).xyz;

	// ssao
	vec3 ssao_tangent = normalize(ssao_noise_vec - normal * dot(ssao_noise_vec, normal));
	vec3 ssao_bitangent = cross(normal, ssao_tangent);
	mat3 ssao_TBN = mat3(ssao_tangent, ssao_bitangent, normal);

	float occlusion = 0.0;
	for (int i = 0; i < 16; ++i)
	{
		vec3 sample_pos = ssao_TBN * ssao_samples[i];
		sample_pos = FragPos + sample_pos * ssao_radius;

		vec4 offset = vec4(sample_pos, 1.0);
		offset = projection * offset;
		offset.xyz /= offset.w;
		offset.xyz = offset.xyz * 0.5 + 0.5;

		float sample_depth = texture(gPosition, offset.xy).z;
		float range_check = smoothstep(0.0, 1.0, ssao_radius / abs(FragPos.z - sample_depth));
		occlusion += (sample_depth >= sample_pos.z + ssao_bias ? 1.0 : 0.0) * range_check;
	}
	occlusion = 1.0 - (occlusion / 16.0);

	vec3 result = vec3(0.0, 0.0, 0.0);

	result += calcDirLight(dirlight, normal, viewDir);
	
	result *= albedo_spec.xyz * occlusion;

	/*if (FragPos == vec4(0.0, 0.0, 0.0, 1.0))
		FragColor = vec4(0.61, 0.88, 1.0, 1.0);
	else*/
		FragColor = vec4(result, 1.0);
}