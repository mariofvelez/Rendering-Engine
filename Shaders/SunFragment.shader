#version 430 core

out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;

uniform sampler2D uTexture;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = texture(uTexture, TxCoord).xyz;// vec3(0.388, 0.851, 0.035);

	FragColor = vec4(result, 1.0);
}