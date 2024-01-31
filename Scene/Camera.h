#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Renderer/Light.h"

class Camera
{
public:
	glm::vec3 m_pos;
	glm::vec3 m_front;

	glm::mat4 view;
	glm::mat4 projection;

	float speed;
	float yaw;
	float pitch;

	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up) : m_pos(pos), m_front(front), m_up(up), view(1.0f), projection(1.0f), m_FOV(60.0f), speed(10.0f), pitch(0), near(0.1f), far(500.0f)
	{
		yaw = atan2f(m_front.y, m_front.x);
		updateView();
	}
	~Camera() {}

	void updateView()
	{
		view = glm::lookAt(m_pos, m_pos + m_front, m_up);
	}

	void updateProjection(float aspect)
	{
		projection = glm::perspective(glm::radians(m_FOV), aspect, near, far);
	}

	void uniformView(Shader* shader)
	{
		unsigned int view_loc = shader->uniformLoc("view");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
	}

	void uniformViewPos(Shader* shader)
	{
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
		pitch += radians;
		if (pitch < -glm::half_pi<float>())
			pitch = -glm::half_pi<float>() + 0.0001f;
		if (pitch > glm::half_pi<float>())
			pitch = glm::half_pi<float>() - 0.0001f;
		m_front.x = cosf(yaw) * cosf(pitch);
		m_front.y = sinf(yaw) * cosf(pitch);
		m_front.z = sinf(pitch);
	}
	void rotateYaw(float radians)
	{
		yaw = atan2f(m_front.y, m_front.x);
		yaw += radians;
		m_front.x = cosf(yaw) * cosf(pitch);
		m_front.y = sinf(yaw) * cosf(pitch);
		m_front.z = sinf(pitch);
	}
	void setFacingDirection(glm::vec3 dir)
	{

	}
	void updateCSM(Shader* shader, glm::ivec2 resolution, glm::mat4* shadow_map_matrices, DirLight* light)
	{
		int m = 4; // number of planes

		std::vector<float> plane_depths;
		plane_depths.reserve(m);
		plane_depths.emplace_back(near);

		// calculating plane depths
		for (int i = 1; i < m - 1; ++i)
		{
			float Ci_log = near * pow(far / near, (float)i / (float)m);
			float Ci_uniform = near + (far - near) * (float)i / (float)m;
			float Ci = (Ci_log + Ci_uniform) * 0.5f;
			plane_depths.emplace_back(Ci);

			shader->setFloat("cascaded_depths[" + std::to_string(Ci) + "]", Ci);
		}
		plane_depths.emplace_back(far);

		glm::mat4 inverse = glm::inverse(projection * view);
		std::vector<glm::vec4> frustum_vertices;
		frustum_vertices.reserve(plane_depths.size() * 4);

		for(unsigned int i = 0; i < plane_depths.size(); ++i)
		{
			float depth = plane_depths[i];
			frustum_vertices.emplace_back(inverse * glm::vec4(-1.0f,  1.0f, depth, 1.0f));
			frustum_vertices.emplace_back(inverse * glm::vec4( 1.0f,  1.0f, depth, 1.0f));
			frustum_vertices.emplace_back(inverse * glm::vec4( 1.0f, -1.0f, depth, 1.0f));
			frustum_vertices.emplace_back(inverse * glm::vec4(-1.0f, -1.0f, depth, 1.0f));
		}

		// perspective division and project onto light view
		glm::mat4 light_view = glm::lookAt(m_pos, m_pos + light->direction, m_up);
		for (unsigned int i = 0; i < frustum_vertices.size(); ++i)
		{
			frustum_vertices[i] /= frustum_vertices[i].w;
			frustum_vertices[i] = light_view * frustum_vertices[i];
		}

		// update matrices
		glm::vec3 dir = light->direction;
		for (int i = 0; i < 3; ++i)
		{
			// find bounding box
			float left = frustum_vertices[i * 4].x;
			float right = frustum_vertices[i * 4].x;
			float bottom = frustum_vertices[i * 4].y;
			float top = frustum_vertices[i * 4].y;
			float front = frustum_vertices[i * 4].z;
			float back = frustum_vertices[i * 4].z;

			for (int j = 1; j < 8; ++j)
			{
				glm::vec3 vertex = frustum_vertices[i * 4 + j];
				if (vertex.x < left) left = vertex.x;
				if (vertex.x > right) right = vertex.x;
				if (vertex.y < bottom) bottom = vertex.y;
				if (vertex.y > top) top = vertex.y;
				if (vertex.z < back) back = vertex.z;
				if (vertex.z > front) front = vertex.z;
			}

			shadow_map_matrices[i] = glm::ortho(left, right, bottom, top, back, front) * light_view;
		}
	}
	void processInput(GLFWwindow* window, float dt)
	{
		float camera_speed = speed * dt;
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera_speed *= 3.0f;

		glm::vec3 forward = glm::normalize(glm::vec3(m_front.x, m_front.y, 0.0f)) * camera_speed;
		glm::vec3 side = glm::normalize(glm::cross(forward, m_up)) * camera_speed;

		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			rotateYaw(0.25 * camera_speed);
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			rotateYaw(-0.25 * camera_speed);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			rotatePitch(0.25 * camera_speed);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			rotatePitch(-0.25 * camera_speed);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			move(forward);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			move(-side);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			move(-forward);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			move(side);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			move(glm::vec3(0.0f, 0.0f, camera_speed));
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			move(glm::vec3(0.0f, 0.0f, -camera_speed));

		updateView();
	}

private:
	glm::vec3 m_up;
	float m_FOV;

	float near;
	float far;
};