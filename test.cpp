#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <random>

#include "stb_image.h"

#include "Scene/Scene.h"
#include "Scene/Terrain.h"
#include "Renderer/Light.h"
#include "Renderer/Bloom.h"

#include "Debug.h"

#define G_POSITION_TEXTURE 2
#define G_NORMAL_TEXTURE 3
#define G_ALBEDO_SPEC_TEXTURE 4
#define SSAO_NOISE_TEXTURE 5
#define SSAO_BLUR_INPUT_TEXTURE 5
#define SSAO_TEXTURE 5
#define HDR_COLOR_TEXTURE 5
#define BLOOM_COLOR_TEXTURE 6

float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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
	
	Camera* camera = new Camera(glm::vec3(0.0f, -30.0f, 68.0f),
								glm::vec3(0.0f, 1.0f, 0.0f),
								glm::vec3(0.0f, 0.0f, 1.0f));
	
	Terrain* terrain = new Terrain(camera);
	
	//Scene* scene = new Scene(camera);

	// add light
	DirLight* light = new DirLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::normalize(glm::vec3(0.3f, 0.4f, -1.0f)), 0.01f, false);
	
	/*light->uniformShader(scene->shader, "dirlight");

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
	ro->move(0.0f, 0.0f, 0.0f); */

	// chunk test
	std::string filenames[] = {
		"res/grass2.png",
		"res/sand.png",
		"res/stone.png",
		"res/snow.png",
		"res/mud.png",
		"res/magma.png",
		"res/darkstone.png",
		"res/log.png",
		"res/water_temp3.png",
		"res/water_temp4.png"
	};
	Chunk::createTextureArray(filenames, 10, terrain->terrain_shader);
	Chunk::createVertexBuffer();

	for (int x = -2; x < 2; ++x)
	{
		for (int y = -2; y < 2; ++y)
		{
			for (int z = 0; z < 4; ++z)
			{
				//std::cout << "loading chunk: " << x << ", " << y << ", " << z << std::endl;
				terrain->loadChunk(x, y, z);
			}
		}
	}

	/*Chunk* chunk = new Chunk(glm::vec3(0.0f, 0.0f, 0.0f), 0);
	srand(0);
	for (unsigned int i = 0; i < Chunk::length; ++i)
	{
		int num = rand() % 3;
		if (num == 0)
			chunk->m_data[i] = rand() % 4;
		else
			chunk->m_data[i] = 0;
	}
	chunk->updateMesh();*/

	bool running = true;
	bool* running_ptr = &running;
	std::thread update_chunk_thread([running_ptr, terrain]() {

		using namespace std::literals::chrono_literals;

		while (*running_ptr)
		{
			//std::cout << "updating chunks" << std::endl;
			terrain->updateLoadedChunks(running_ptr);
			std::this_thread::sleep_for(0.1s);
		}
	});

	//unsigned int block_index;
	//block_index = glGetProgramResourceIndex(scene->shader->m_ID, GL_SHADER_STORAGE_BLOCK, "blockBuffer");

	// Deferred rendering
	unsigned int g_buffer;
	glGenFramebuffers(1, &g_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
	unsigned int g_position, g_normal, g_color_spec;

	// position buffer
	glGenTextures(1, &g_position);
	glBindTexture(GL_TEXTURE_2D, g_position);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);

	// normal buffer
	glGenTextures(1, &g_normal);
	glBindTexture(GL_TEXTURE_2D, g_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);

	// color and specular buffer
	glGenTextures(1, &g_color_spec);
	glBindTexture(GL_TEXTURE_2D, g_color_spec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_color_spec, 0);

	// atachments used for rendering
	unsigned int attachments[3] = {
		GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
	};
	glDrawBuffers(3, attachments);

	// add depth buffer
	unsigned int rbo_depth;
	glGenRenderbuffers(1, &rbo_depth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

	// check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "G buffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// quad
	float quad_vertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	unsigned int quadVAO;
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int quad_indices[6] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int quadEBO;
	glGenBuffers(1, &quadEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);

	// lighting buffer
	unsigned int lighting_buffer;
	glGenFramebuffers(1, &lighting_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, lighting_buffer);

	unsigned int hdr_color;
	glGenTextures(1, &hdr_color);
	glBindTexture(GL_TEXTURE_2D, hdr_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_color, 0);

	Shader* lighting_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/LightingFragment.shader");
	lighting_shader->use();
	lighting_shader->setInt("gPosition", G_POSITION_TEXTURE);
	lighting_shader->setInt("gNormal", G_NORMAL_TEXTURE);
	lighting_shader->setInt("gAlbedoSpec", G_ALBEDO_SPEC_TEXTURE);
	lighting_shader->setInt("ssao", SSAO_TEXTURE);

	light->uniformShader(lighting_shader, &camera->view, "dirlight");

	// SSAO
	std::uniform_real_distribution<float> rand_floats(0.0f, 1.0f);
	std::default_random_engine generator;
	std::vector<glm::vec3> ssao_kernel;
	for (unsigned int i = 0; i < 16; ++i)
	{
		glm::vec3 sample(
			rand_floats(generator) * 2.0f - 1.0f,
			rand_floats(generator) * 2.0f - 1.0f,
			rand_floats(generator)
		);
		sample = glm::normalize(sample);
		sample *= rand_floats(generator);
		float scale = (float)i / 16.0f;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssao_kernel.push_back(sample);
	}

	std::vector<glm::vec3> ssao_noise;
	for (unsigned int i = 0; i < 16; ++i)
	{
		glm::vec3 noise(
			rand_floats(generator) * 2.0f - 1.0f,
			rand_floats(generator) * 2.0f - 1.0f,
			0.0f
		);
		ssao_noise.push_back(noise);
	}

	unsigned int noise_texture;
	glGenTextures(1, &noise_texture);
	glBindTexture(GL_TEXTURE_2D, noise_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_noise[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// ssao buffer
	unsigned int ssao_buffer;
	glGenFramebuffers(1, &ssao_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ssao_buffer);

	unsigned int ssao_color;
	glGenTextures(1, &ssao_color);
	glBindTexture(GL_TEXTURE_2D, ssao_color);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width, screen_height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_color, 0);

	Shader* ssao_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/SSAOFragment.shader");
	ssao_shader->use();
	ssao_shader->setInt("gPosition", G_POSITION_TEXTURE);
	ssao_shader->setInt("gNormal", G_NORMAL_TEXTURE);
	ssao_shader->setInt("ssaoNoise", SSAO_NOISE_TEXTURE);

	for (unsigned int i = 0; i < 16; ++i)
	{
		ssao_shader->setVec3("ssao_samples[" + std::to_string(i) + ']', ssao_kernel[i]);
	}

	// ssao blur buffer
	unsigned int ssao_blur_buffer;
	glGenFramebuffers(1, &ssao_blur_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_buffer);

	unsigned int ssao_blur;
	glGenTextures(1, &ssao_blur);
	glBindTexture(GL_TEXTURE_2D, ssao_blur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width, screen_height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_blur, 0);

	Shader* ssao_blur_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/SSAOBlurFragment.shader");
	ssao_blur_shader->use();
	ssao_blur_shader->setInt("ssaoInput", SSAO_BLUR_INPUT_TEXTURE);

	Shader* hdr_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/HDRFragment.shader");
	hdr_shader->use();
	hdr_shader->setInt("hdrColor", HDR_COLOR_TEXTURE);
	hdr_shader->setInt("bloomColor", BLOOM_COLOR_TEXTURE);

	// bloom
	BloomBuffer bloom_buffer;
	glm::ivec2 src_viewport_size(screen_width, screen_height);
	glm::vec2 src_viewport_size_float((float)screen_width, (float)screen_height);
	Shader* bloom_downsample_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/BloomDownsamplingFragment.shader");
	Shader* bloom_upsample_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/BloomUpsamplingFragment.shader");

	const unsigned int num_bloom_mips = 5;
	bool bloom_buffer_status = bloom_buffer.init(screen_width, screen_height, num_bloom_mips);
	if (!bloom_buffer_status)
	{
		std::cout << "Failed to initialize bloom buffer!";
		return 1;
	}

	const float bloom_filter_radius = 0.005f;

	bloom_downsample_shader->use();
	bloom_downsample_shader->setInt("srcTexture", 5);

	bloom_upsample_shader->use();
	bloom_upsample_shader->setInt("srcTexture", 5);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, g_position);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, g_normal);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, g_color_spec);

	// cascaded shadow map
	light->createCascadedShadowBuffer(3);

	terrain->updateTestCapture();

	float delta_time = 0.0f;
	float last_frame = 0.0f;

	float delta_time_list[100];
	for (unsigned int i = 0; i < 100; ++i)
		delta_time_list[i] = 0;
	int dt_index = 0;
	float dt_avg = 0;

	glEnable(GL_DEPTH_TEST);

	bool is_c_pressed = false;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		bool key_c = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;

		terrain->updateTestCapture();
		if (is_c_pressed != key_c) // key changed
		{
			//std::cout << "captured camera" << std::endl;
		}

		is_c_pressed = key_c;

		float current_frame = (float) glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// camera controls
		camera->processInput(window, delta_time);

		debug_start(glfwGetTime(), 0);
		//glClearColor(0.61f, 0.88f, 1.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/*float angle = current_frame / 10.0f;
		float side = cosf(angle);
		float vertical = sinf(angle);

		light->direction.x = cosf(0.5f) * side;
		light->direction.y = sinf(0.5f) * side;
		light->direction.z = vertical;*/

		terrain->draw();

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glClearColor(0.61f, 0.88f, 1.0f, 1.0f);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		// ssao
		glBindFramebuffer(GL_FRAMEBUFFER, ssao_buffer);
		glClear(GL_COLOR_BUFFER_BIT);
		ssao_shader->use();
		camera->uniformProjection(ssao_shader);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, noise_texture);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// ssao blur
		glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_buffer);
		glClear(GL_COLOR_BUFFER_BIT);
		ssao_blur_shader->use();
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ssao_color);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// cascaded shadow map
		/*
		* for each cascade
		* - determine which chunks are visible occluders
		* - set near and far planes closest together for precision
		* - render all chunks to the cascade buffer
		*/
		camera->updateCSM(lighting_shader, light->shadow_matrices, light);

		for (int i = 0; i < light->num_cascades; ++i)
		{
			glm::mat4& view = light->shadow_matrices[i];

			// terrain->draw(view);
		}

		// lighting
		glBindFramebuffer(GL_FRAMEBUFFER, lighting_buffer);
		glClear(GL_COLOR_BUFFER_BIT);
		lighting_shader->use();
		light->uniformShader(lighting_shader, &camera->view, "dirlight");

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ssao_blur);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		debug_end(glfwGetTime(), 0);

		// bloom
		bloom_buffer.bindForWriting();

		const std::vector<BloomMip>& mip_chain = bloom_buffer.mipChain();

		// downsample
		bloom_downsample_shader->use();
		bloom_downsample_shader->setVec2("srcResolution", src_viewport_size_float);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdr_color);

		for (unsigned int i = 0; i < mip_chain.size(); ++i)
		{
			const BloomMip& mip = mip_chain[i];
			glViewport(0, 0, mip.size.x, mip.size.y);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			bloom_downsample_shader->setVec2("srcResolution", mip.size);

			glBindTexture(GL_TEXTURE_2D, mip.texture);
		}

		// upsample
		bloom_upsample_shader->use();
		bloom_upsample_shader->setFloat("filterRadius", bloom_filter_radius);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		for (int i = mip_chain.size() - 1; i > 0; --i)
		{
			const BloomMip& mip = mip_chain[i];
			const BloomMip& next_mip = mip_chain[i - 1];

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, mip.texture);

			glViewport(0, 0, next_mip.size.x, next_mip.size.y);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, next_mip.texture, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glDisable(GL_BLEND);

		glViewport(0, 0, screen_width, screen_height);

		// hdr and gamma
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		hdr_shader->use();

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdr_color);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mip_chain[0].texture);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		glBindVertexArray(0);
		

		delta_time_list[dt_index] = debug_time(0);
		dt_index++;
		dt_index = dt_index % 100;
		dt_avg = 0;
		for (unsigned int i = 0; i < 100; ++i)
			dt_avg += delta_time_list[i];
		dt_avg *= 0.01f;

		dlogln("time avg: " << dt_avg);
		//std::cout << "error: " << glGetError() << std::endl;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	running = false;
	update_chunk_thread.join();

	delete(camera);
	//delete(scene);
	delete(light);
	delete(terrain);
	delete(lighting_shader);

	glfwTerminate();
	return 0;
}