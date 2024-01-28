#version 430 core

out float FragColor;

in vec2 TexCoord;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D ssaoNoise;

uniform vec3 ssao_samples[16];
uniform mat4 projection;

const float ssao_radius = 2.0;
const float ssao_bias = 0.025;

const vec2 noiseScale = vec2(900.0 / 4.0, 600.0 / 4.0);

void main()
{
	vec3 FragPos = texture(gPosition, TexCoord).xyz;
	vec3 normal = texture(gNormal, TexCoord).rgb;
	vec3 ssao_noise_vec = texture(ssaoNoise, TexCoord * noiseScale).xyz;

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

	FragColor = occlusion;
}