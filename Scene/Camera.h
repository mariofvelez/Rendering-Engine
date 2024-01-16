#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	glm::vec3 m_pos;
	glm::vec3 m_front;

	glm::mat4 view;
	glm::mat4 projection;

	float speed;

	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up) : m_pos(pos), m_front(front), m_up(up), view(1.0f), projection(1.0f), m_FOV(45.0f), speed(10.0f)
	{
		updateView();
	}
	~Camera() {}

	void updateView()
	{
		view = glm::lookAt(m_pos, m_pos + m_front, m_up);
	}

	void updateProjection(float aspect)
	{
		projection = glm::perspective(glm::radians(m_FOV), aspect, 0.1f, 100.0f);
	}

	void uniformView(Shader* shader)
	{
		unsigned int view_loc = shader->uniformLoc("view");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

		shader->setVec3("viewPos", m_pos);
	}

	void uniformProjection(Shader* shader)
	{
		unsigned int proj_loc = shader->uniformLoc("projection");
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(projection));
	}

	void move(glm::vec3 move)
	{
		m_pos += move;
	}
	void rotatePitch(float radians)
	{

	}
	void rotateYaw(float radians)
	{
		float angle = atan2f(m_front.y, m_front.x);
		angle += radians;
		m_front.x = cosf(angle);
		m_front.y = sinf(angle);
		m_front = glm::normalize(m_front);
	}
	void setFacingDirection(glm::vec3 dir)
	{

	}
	void processInput(GLFWwindow* window, float dt)
	{
		float camera_speed = speed * dt;

		glm::vec3 forward = glm::normalize(m_front) * camera_speed;
		glm::vec3 side = glm::normalize(glm::cross(forward, m_up)) * camera_speed;

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			rotateYaw(0.25 * camera_speed);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			rotateYaw(-0.25 * camera_speed);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			move(forward);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			move(-side);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			move(-forward);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			move(side);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			move(glm::vec3(0.0f, 0.0f, camera_speed));
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			move(glm::vec3(0.0f, 0.0f, -camera_speed));

		updateView();
	}

private:
	glm::vec3 m_up;
	float m_FOV;
};