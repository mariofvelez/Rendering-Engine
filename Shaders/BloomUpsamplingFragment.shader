#version 430 core

layout (location = 0) out vec3 upsample;

in vec2 TexCoord;

uniform sampler2D srcTexture;
uniform float filterRadius;

void main()
{
	float x = filterRadius;
	float y = filterRadius;

    vec3 a = texture(srcTexture, vec2(TexCoord.x - x, TexCoord.y + y)).rgb;
    vec3 b = texture(srcTexture, vec2(TexCoord.x,     TexCoord.y + y)).rgb;
    vec3 c = texture(srcTexture, vec2(TexCoord.x + x, TexCoord.y + y)).rgb;

    vec3 d = texture(srcTexture, vec2(TexCoord.x - x, TexCoord.y)).rgb;
    vec3 e = texture(srcTexture, vec2(TexCoord.x,     TexCoord.y)).rgb;
    vec3 f = texture(srcTexture, vec2(TexCoord.x + x, TexCoord.y)).rgb;

    vec3 g = texture(srcTexture, vec2(TexCoord.x - x, TexCoord.y - y)).rgb;
    vec3 h = texture(srcTexture, vec2(TexCoord.x,     TexCoord.y - y)).rgb;
    vec3 i = texture(srcTexture, vec2(TexCoord.x + x, TexCoord.y - y)).rgb;

    upsample = e * 4.0;
    upsample += (b + d + f + h) * 2.0;
    upsample += (a + c + g + i);
    upsample *= 1.0 / 16.0;
}