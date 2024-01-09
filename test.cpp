#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "Scene/Scene.h"
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

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
	
	Camera* camera = new Camera(glm::vec3(0.0f, -5.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f));
	
	Scene* scene = new Scene(camera);

	// add light
	DirLight* light = new DirLight(glm::vec3(1.0f, 1.0f, 0.7f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0), 0.5f, false);

	light->uniformShader(scene->shader, "dirlight");

	// add cube
	float vertices[] = {
	 0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f, 0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, 0.0f,  1.0f,  0.0f
	};

	unsigned int indices[36];
	for (unsigned int i = 0; i < 36; ++i)
		indices[i] = i;

	unsigned int VBO;
	glGenBuffers(1, &VBO);

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int EBO;
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	Scene::ro_params.VAO = VAO;
	Scene::ro_params.num_elements = 36;

	RenderObject* ro = scene->addRenderObject();
	ro->move(1.0f, 0.0f, 0.0f);

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

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete(camera);
	delete(scene);
	delete(light);

	glfwTerminate();
	return 0;
}