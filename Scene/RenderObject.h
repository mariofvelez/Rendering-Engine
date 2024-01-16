#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Renderer/Shader.h"

struct RenderObjectAttribParameters
{
	unsigned int position_loc;
	unsigned int color_loc;
	unsigned int normal_loc;
	unsigned int tex_coord_loc;
};
class RenderObject
{
public:
	unsigned int m_VAO;
	unsigned int m_num_elements;
	unsigned int m_texture;

	glm::mat4 model;

	RenderObject(unsigned int VAO, unsigned int num_elements, unsigned int texture) : m_VAO(VAO), m_num_elements(num_elements), m_texture(texture), model(1.0f)
	{
		
	}
	RenderObject(const std::string& filename) : model(1.0f)
	{
		std::fstream file;

		file.open(filename);

		if (file.is_open())
		{
			std::string line;

			while (std::getline(file, line))
			{
				// parse each line
				if (line.length() == 0)
					continue;

				std::stringstream sstream(line);
				std::string str;
				std::getline(sstream, str, ' ');
				std::vector<std::string> params;

				if (str == "v");
				if (str == "vt");
				if (str == "vn");
				if (str == "")

				while (std::getline(sstream, str, ' '))
				{
					params.emplace_back(str);
				}
			}
		}
		else
		{
			// error loading file
		}
	}
	~RenderObject() {}

	void move(float x, float y, float z)
	{
		model = glm::translate(model, glm::vec3(x, y, z));
	}

	void setModelUniform(Shader* shader)
	{
		model_uniform_loc = shader->uniformLoc("model");
		glUniformMatrix4fv(model_uniform_loc, 1, GL_FALSE, glm::value_ptr(model));
	}

	void setTextureLocations(Shader* shader)
	{
		unsigned int loc = shader->uniformLoc("uTexture");
		glUniform1i(loc, 0);
	}
private:
	unsigned int model_uniform_loc = 0;
};