#pragma once
#include <vector>

#include "RenderObject.h"
#include "ShaderManager.h"
#include "Camera.h"

struct RenderObjectParameters
{
	unsigned int VAO;
	unsigned int num_elements;
	bool has_vertex_colors;
	bool has_vertex_normals;
	bool has_vertex_bitangents;
	bool is_transparent;
	unsigned int diffuse_texture;
	unsigned int specular_texture;
	unsigned int normal_texture;
	unsigned int bump_texture;
};

class Scene
{
public:
	Camera* curr_camera;
	Shader* shader;
	
	static RenderObjectParameters ro_params;

	Scene(Camera* curr_camera) : curr_camera(curr_camera), should_recompile_shader(false)
	{
		shader = new Shader("Shaders/Vertex.shader", "Shaders/Fragment.shader");
		shader->use();

		curr_camera->uniformView(shader);

		curr_camera->updateProjection(9.0f / 6.0f);
		curr_camera->uniformProjection(shader);
	}
	~Scene()
	{
		delete(shader);
	}

	void RenderScene()
	{
		shader->use();

		curr_camera->uniformView(shader);

		glActiveTexture(GL_TEXTURE0);
		for (unsigned int i = 0; i < m_RenderObjects.size(); ++i)
		{
			RenderObject* ro = &m_RenderObjects[i];
			ro->setModelUniform(shader);

			glBindVertexArray(ro->m_VAO);

			glDrawElements(GL_TRIANGLES, ro->m_num_elements, GL_UNSIGNED_INT, 0);
		}
	}

	void setShouldRecompileShader()
	{
		should_recompile_shader = true;
	}

	RenderObject* addRenderObject()
	{
		m_RenderObjects.emplace_back(ro_params.VAO, ro_params.num_elements);
		return &m_RenderObjects[m_RenderObjects.size() - 1];
	}

private:
	ShaderManager m_ShaderManager;

	bool should_recompile_shader;

	std::vector<RenderObject> m_RenderObjects;
};

RenderObjectParameters Scene::ro_params;