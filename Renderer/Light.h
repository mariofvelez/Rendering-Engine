#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>

enum class LightType
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct Light
{
	const LightType type;

	glm::vec3 color;
	
	glm::vec3 position;
	glm::vec3 direction;

	float ambient_strength;

	// shadow mapping
	unsigned int shadow_fbo;
	unsigned int shadow_map;

	bool casts_shadow;

	Light(LightType type, glm::vec3 color, glm::vec3 position, glm::vec3 direction, float ambient_strength, bool casts_shadow) : color(color), position(position), direction(direction), ambient_strength(ambient_strength), casts_shadow(casts_shadow), shadow_fbo(0), shadow_map(0), type(type) {};

	void setColor(float r, float g, float b)
	{
		color.x = r;
		color.y = g;
		color.z = b;
	}

	virtual void uniformShader(Shader* shader, glm::mat4* view, const std::string& uniform_name)
	{
		
	}
};

struct DirLight : public Light
{
	unsigned int shadow_width = 2048;
	unsigned int shadow_height = 2048;

	DirLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float ambient_strength, bool casts_shadow) : Light(LightType::DIRECTIONAL_LIGHT, color, position, direction, ambient_strength, casts_shadow) {}

	~DirLight() {}

	void uniformShader(Shader* shader, glm::mat4* view, const std::string& uniform_name)
	{
		glm::vec3 dir = glm::mat3(*view) * direction;
		shader->use();
		shader->setVec3(uniform_name + ".direction", dir);

		shader->setVec3(uniform_name + ".ambient", color.x * ambient_strength, color.y * ambient_strength, color.z * ambient_strength);
		shader->setVec3(uniform_name + ".diffuse", color);
		shader->setVec3(uniform_name + ".specular", color);
	}
};