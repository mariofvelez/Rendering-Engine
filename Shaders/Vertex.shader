#version 430 core

layout (location = 0) in vec3 aPos; // vertex position
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aBlockLocation;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;
out float BlockLocation;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// render in view space

	mat4 local_to_view = view * model;

	vec4 view_pos = local_to_view * vec4(aPos, 1.0); // position in view space
	gl_Position = projection * view_pos;
	Normal = mat3(local_to_view) * aNormal;
	FragPos = view_pos.xyz;
	TexCoord = aTexCoord;
	BlockLocation = aBlockLocation;
}