#pragma once

#include "../Renderer/Shader.h"

class DayNightCycle
{
	Shader* sun_shader;
	unsigned int sun_model_loc;

	unsigned int sunVAO;
	unsigned int sun_texture;

public:
	DayNightCycle()
	{
		sun_shader = new Shader("SunVertex.shader", "SunFragment.shader");
		sun_shader->use();

		sun_model_loc = sun_shader->uniformLoc("model");

		float sun_vertices[] = {
			-0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f, 1.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 1.0f
		};

		unsigned int sun_indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		glGenVertexArrays(1, &sunVAO);
		glBindVertexArray(sunVAO);

		unsigned int sunVBO;
		glGenBuffers(1, &sunVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sunVBO);

		glBufferData(GL_ARRAY_BUFFER, sizeof(sun_vertices), sun_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		unsigned int sunEBO;
		glGenBuffers(1, &sunEBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sun_indices), sun_indices, GL_STATIC_DRAW);


	}
	~DayNightCycle()
	{
		delete(sun_shader);
	}
	void draw()
	{
		glDisable(GL_DEPTH_TEST);

	}
};