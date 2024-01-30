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

	virtual void createShadowBuffer()
	{

	}
};

struct DirLight : public Light
{
	unsigned int shadow_width = 512;
	unsigned int shadow_height = 512;

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

	void createShadowBuffer()
	{
		glGenFramebuffers(1, &shadow_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);

		glGenTextures(1, &shadow_map);
		glBindTexture(GL_TEXTURE_2D, shadow_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void createCascadedShadowBuffer(int num_cascades)
	{
		glGenFramebuffers(1, &shadow_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);

		glGenTextures(1, &shadow_map);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadow_map);

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, shadow_width, shadow_height, num_cascades, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border_color);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadow_map, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Error: CSM Framefuffer not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};