#pragma once

#include "Camera.h"
#include "Terrain.h"

class World
{
public:
	Camera* camera;

	Terrain* terrain;

	DirLight* light;

	World()
	{
		camera = new Camera(glm::vec3(0.0f, 0.0f, 35.0f),
							glm::vec3(0.0f, 1.0f, 0.0f),
							glm::vec3(0.0f, 0.0f, 1.0f));

		terrain = new Terrain(camera);

		light = new DirLight(glm::vec3(2.0f, 2.0f, 2.0f), // color
							 glm::vec3(0.0f, 0.0f, 0.0f), // pos
							 glm::normalize(glm::vec3(0.3f, 0.4f, -1.0f)), // direction
							 0.01f, false);
	}
	~World()
	{
		delete(camera);
		delete(terrain);
		delete(light);
	}
};