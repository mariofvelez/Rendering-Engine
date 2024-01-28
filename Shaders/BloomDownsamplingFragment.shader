#version 430 core

layout (location = 0) out vec3 downsample;

in vec2 TexCoord;

uniform sampler2D srcTexture;
uniform vec2 srcResolution;

void main()
{
	vec2 src_tex_size = 1.0 / srcResolution;
	float x = src_texel_size.x;
	float y = src_texel_size.y;

	vec3 a = texture(srcTexture, vec2(TexCoord.x - 2 * x, TexCoord.y + 2 * y)).rgb;
    vec3 b = texture(srcTexture, vec2(TexCoord.x, TexCoord.y + 2 * y)).rgb;
    vec3 c = texture(srcTexture, vec2(TexCoord.x + 2 * x, TexCoord.y + 2 * y)).rgb;

    vec3 d = texture(srcTexture, vec2(TexCoord.x - 2 * x, TexCoord.y)).rgb;
    vec3 e = texture(srcTexture, vec2(TexCoord.x, TexCoord.y)).rgb;
    vec3 f = texture(srcTexture, vec2(TexCoord.x + 2 * x, TexCoord.y)).rgb;

    vec3 g = texture(srcTexture, vec2(TexCoord.x - 2 * x, TexCoord.y - 2 * y)).rgb;
    vec3 h = texture(srcTexture, vec2(TexCoord.x, TexCoord.y - 2 * y)).rgb;
    vec3 i = texture(srcTexture, vec2(TexCoord.x + 2 * x, TexCoord.y - 2 * y)).rgb;

    vec3 j = texture(srcTexture, vec2(TexCoord.x - x, TexCoord.y + y)).rgb;
    vec3 k = texture(srcTexture, vec2(TexCoord.x + x, TexCoord.y + y)).rgb;
    vec3 l = texture(srcTexture, vec2(TexCoord.x - x, TexCoord.y - y)).rgb;
    vec3 m = texture(srcTexture, vec2(TexCoord.x + x, TexCoord.y - y)).rgb;

    downsample = e * 0.125;
    downsample += (a + c + g + i) * 0.03125;
    downsample += (b + d + f + h) * 0.0625;
    downsample += (j + k + l + m) * 0.125;
}