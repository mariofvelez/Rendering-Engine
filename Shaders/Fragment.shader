#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;
in float BlockLocation;

//uniform sampler2D uTexture;
uniform sampler2DArray uTextureArray;

layout(std430, binding = 1) buffer blockBuffer
{
	uint data[16 * 16 * 16];
};

void main()
{
	gPosition = FragPos;

	vec3 norm = normalize(Normal);
	gNormal = norm;

	gAlbedoSpec.rgb = texture(uTextureArray, vec3(TexCoord, data[int(BlockLocation)] - 1)).xyz;
	gAlbedoSpec.a = 0.5f; // change to texture
}