#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <stdlib.h>

#include "stb_image.h"

#include "Scene/World.h"
#include "Renderer/Renderer.h"

#include "Debug.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

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


	// ImGui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


	
	World* world = new World();
	
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
		"res/water_temp4.png",
		"res/lantern.png"
	};
	Chunk::createTextureArray(filenames, 11, world->terrain->terrain_shader);
	Chunk::createVertexBuffer();

	world->terrain->loadChunk(0, 0, 0);

	//for (int x = -2; x < 2; ++x)
	//{
	//	for (int y = -2; y < 2; ++y)
	//	{
	//		for (int z = 0; z < 1; ++z)
	//		{
	//			//std::cout << "loading chunk: " << x << ", " << y << ", " << z << std::endl;
	//			terrain->loadChunk(x, y, z);
	//		}
	//	}
	//}

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

	//bool running = false;
	//bool* running_ptr = &running;
	//std::thread update_chunk_thread([running_ptr, terrain]() {

	//	using namespace std::literals::chrono_literals;

	//	while (*running_ptr)
	//	{
	//		//std::cout << "updating chunks" << std::endl;
	//		terrain->updateLoadedChunks(running_ptr);
	//		std::this_thread::sleep_for(0.1s);
	//	}
	//});

	//unsigned int block_index;
	//block_index = glGetProgramResourceIndex(scene->shader->m_ID, GL_SHADER_STORAGE_BLOCK, "blockBuffer");

	Renderer* renderer = new Renderer(screen_width, screen_height);	

	world->terrain->updateTestCapture();

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
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::ShowDemoWindow();

		processInput(window);

		bool key_c = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;

		world->terrain->updateTestCapture();
		if (is_c_pressed != key_c) // key changed
		{
			//std::cout << "captured camera" << std::endl;
		}

		is_c_pressed = key_c;

		float current_frame = (float) glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		// camera controls
		world->camera->processInput(window, delta_time);

		debug_start(glfwGetTime(), 0);
		//glClearColor(0.61f, 0.88f, 1.0f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		renderer->render(world);

		

		

		

		// cascaded shadow map
		/*
		* for each cascade
		* - determine which chunks are visible occluders
		* - set near and far planes closest together for precision
		* - render all chunks to the cascade buffer
		*/
		//camera->updateCSM(lighting_shader, light->shadow_matrices, light);

		//glViewport(0, 0, light->shadow_width, light->shadow_height);
		//glBindFramebuffer(GL_FRAMEBUFFER, light->shadow_fbo);
		//for (int i = 0; i < light->num_cascades; ++i)
		//{
		//	glClear(GL_DEPTH_BUFFER_BIT);

		//	glm::mat4& view = light->shadow_matrices[i];

		//	// terrain->draw(view);
		//}

		//glViewport(0, 0, screen_width, screen_height);
		

		delta_time_list[dt_index] = debug_time(0);
		dt_index++;
		dt_index = dt_index % 100;
		dt_avg = 0;
		for (unsigned int i = 0; i < 100; ++i)
			dt_avg += delta_time_list[i];
		dt_avg *= 0.01f;

		dlogln("time avg: " << dt_avg);
		//std::cout << "error: " << glGetError() << std::endl;

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	/*running = false;
	update_chunk_thread.join();*/

	delete(renderer);
	delete(world);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}