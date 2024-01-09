#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Renderer/Shader.h"

class RenderObject
{
public:
	unsigned int m_VAO;
	unsigned int m_num_elements;

	glm::mat4 model;

	RenderObject(unsigned int VAO, unsigned int num_elements) : m_VAO(VAO), m_num_elements(num_elements), model(1.0f)
	{
		
	};
	~RenderObject() {};

	void move(float x, float y, float z)
	{
		model = glm::translate(model, glm::vec3(x, y, z));
	}

	void setModelUniform(Shader* shader)
	{
		model_uniform_loc = shader->uniformLoc("model");
		glUniformMatrix4fv(model_uniform_loc, 1, GL_FALSE, glm::value_ptr(model));
	}
private:
	unsigned int model_uniform_loc = 0;
};