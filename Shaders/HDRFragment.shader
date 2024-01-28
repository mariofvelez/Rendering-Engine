#version 430 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D hdrColor;

const float gamma = 2.2;

const float exposure = 0.5;

void main()
{
	vec3 hdr_color = texture(hdrColor, TexCoord).xyz;

	// hdr tone mapping
	vec3 result = vec3(1.0) - exp(-hdr_color * exposure);

	// gamma correction
	result = pow(result, vec3(1.0 / gamma));

	FragColor = vec4(result, 1.0);
}