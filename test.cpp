#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>

#include "stb_image.h"

#include "Scene/Scene.h"
#include "Scene/Chunk.h"
#include "Renderer/Light.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int p[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
float lerp(float t, float a, float b) { return a + t * (b - a); }
float grad(int hash, float x, float y, float z) {
	int h = hash & 15;
	float u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}
float noise(float x, float y, float z) {
	int X = (int)floor(x) & 255,
		Y = (int)floor(y) & 255,
		Z = (int)floor(z) & 255;
	x -= floor(x);
	y -= floor(y);
	z -= floor(z);
	float u = fade(x),
		v = fade(y),
		w = fade(z);
	int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
		B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z),
		grad(p[BA], x - 1, y, z)),
		lerp(u, grad(p[AB], x, y - 1, z),
			grad(p[BB], x - 1, y - 1, z))),
		lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),
			grad(p[BA + 1], x - 1, y, z - 1)),
			lerp(u, grad(p[AB + 1], x, y - 1, z - 1),
				grad(p[BB + 1], x - 1, y - 1, z - 1))));
}
void generateTerrain(unsigned int* data, glm::vec3 offset)
{
	for (int pz = 15; pz >= 0; pz--)
	{
		for (unsigned int py = 0; py < 16; ++py)
		{
			for (unsigned int px = 0; px < 16; ++px)
			{
				unsigned int loc = pz * 256 + py * 16 + px;
				float val = noise(px / 8.0f, py / 8.0f, pz / 8.0f);

				if (val > 0.0f)
				{
					if (pz < 15 && data[loc + 256] > 0)
						data[loc] = 3;
					else
						data[loc] = 1;
				}
				else if (pz < 3)
					data[loc] = 2;
				else
					data[loc] = 0;
			}
		}
	}
}

unsigned int loadTexture(const char* filename)
{
	unsigned int texture;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, n_channels;
	unsigned char* data = stbi_load(filename, &width, &height, &n_channels, 0);
	if (data)
	{
		std::cout << "width: " << width << std::endl;
		std::cout << "height: " << height << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << filename << std::endl;
	}

	stbi_image_free(data);

	return texture;
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	const int screen_width = 900;
	const int screen_height = 600;

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Renderer", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, screen_width, screen_height);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	Camera* camera = new Camera(glm::vec3(0.0f, -30.0f, 5.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	
	Scene* scene = new Scene(camera);

	// add light
	DirLight* light = new DirLight(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(0.3f, 0.1f, -1.0f)), 0.5f, false);

	light->uniformShader(scene->shader, "dirlight");

	// add cube
	float vertices[] = {
		// u: 0.0f, 0.25f, 0.5f, 0.75f, 1.0f
		// v: 0.0f, 0.3333f, 0.6667f, 1.0f
		// bottom
		-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.25f, 0.6667f,
		 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.5f,  0.6667f,
		 0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.5f,  1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.25f, 1.0f,

		// top
		-0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.25f, 0.0f,
		 0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.5f,  0.0f,
		 0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.5f,  0.3333f,
		-0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f, 0.25f, 0.3333f,

		// left
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  0.3333f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.25f, 0.3333f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.25f, 0.6667f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f,  0.6667f,

		// right
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.75f, 0.3333f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.5f,	 0.3333f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.5f,	 0.6667f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.75f, 0.6667f,

		 // front
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.25f, 0.3333f,
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f, 0.5f,  0.3333f,
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.5f,  0.6667f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f, 0.25f, 0.6667f,

		// back
		-0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 1.0f,  0.3333f,
		 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f, 0.75f, 0.3333f,
		 0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 0.75f, 0.6667f,
		-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f, 1.0f,  0.6667f
	};

	unsigned int indices[] = {
		// bottom
		0, 1, 2,
		2, 3, 0,
		// top
		4, 6, 5,
		6, 4, 7,
		// left
		8, 9, 10,
		10, 11, 8,
		// right
		12, 13, 14,
		14, 15, 12,
		// front
		16, 17, 18,
		18, 19, 16,
		// back
		20, 21, 22,
		22, 23, 20
	};

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int texture = loadTexture("res/grass2.png");

	Scene::ro_params.VAO = VAO;
	Scene::ro_params.diffuse_texture = texture;
	Scene::ro_params.num_elements = 36;

	RenderObject* ro = scene->addRenderObject();
	ro->move(0.0f, 0.0f, 0.0f);

	// chunk test
	std::string filenames[] = {"res/grass2.png", "res/sand.png", "res/stone.png"};
	Chunk::createTextureArray(filenames, 3, scene->shader);
	Chunk::createVertexBuffer();

	Chunk* chunk = new Chunk();
	srand(0);
	for (unsigned int i = 0; i < Chunk::length; ++i)
	{
		int num = rand() % 3;
		if (num == 0)
			chunk->m_data[i] = rand() % 4;
		else
			chunk->m_data[i] = 0;
	}
	generateTerrain(chunk->m_data, chunk->m_offset);
	chunk->updateMesh();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunk->m_data_buffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(chunk->m_data), &chunk->m_data, GL_DYNAMIC_READ);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, chunk->m_data_buffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//unsigned int block_index;
	//block_index = glGetProgramResourceIndex(scene->shader->m_ID, GL_SHADER_STORAGE_BLOCK, "blockBuffer");

	float delta_time = 0.0f;
	float last_frame = 0.0f;

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		float current_frame = (float) glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		camera->processInput(window, delta_time);

		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		scene->RenderScene();
		scene->shader->use();
		glBindVertexArray(chunk->m_VAO);
		glDrawElements(GL_TRIANGLES, chunk->m_indices.size(), GL_UNSIGNED_INT, 0); // fix: make chunk->num_elements have the same size

		std::cout << "time: " << delta_time << std::endl;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete(camera);
	delete(scene);
	delete(light);

	glfwTerminate();
	return 0;
}