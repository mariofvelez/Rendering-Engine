#pragma once

#include <vector>

#include "Chunk.h"
#include "Camera.h"
#include "../Renderer/Shader.h"

class Terrain
{
	std::vector<Chunk*> chunks;

public:
	Shader* terrain_shader;
	Camera* camera;

	Terrain(Camera* camera) : camera(camera)
	{
		chunks.reserve(1000);

		terrain_shader = new Shader("Shaders/Vertex.shader", "Shaders/Fragment.shader");
		terrain_shader->use();

		camera->uniformView(terrain_shader);

		camera->updateProjection(9.0f / 6.0f);
		camera->uniformProjection(terrain_shader);
	}
	int p[256] = { 151,160,137,91,90,15,
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
	void generateTerrain(Chunk* chunk)
	{
		
		for (int py = 0; py < Chunk::y_length; ++py)
		{
			for (int px = 0; px < Chunk::x_length; ++px)
			{
				float height = (noise((px + chunk->m_offset.x) / 128.0f,
									 (py + chunk->m_offset.y) / 128.0f,
									  32.7f) / 2.0f) * 256.0f;
				height += (noise((px + chunk->m_offset.x) / 16.0f,
						  (py + chunk->m_offset.y) / 16.0f,
						   32.7f) / 2.0f + 0.5f) * 16.0f;
				for (int pz = Chunk::z_length - 1; pz >= 0; pz--)
				{
					unsigned int loc = pz * Chunk::y_length * Chunk::x_length + py * Chunk::x_length + px;
					float val = noise((px + chunk->m_offset.x) / 16.0f,
									  (py + chunk->m_offset.y) / 16.0f,
									  (pz + chunk->m_offset.z) / 8.0f);

					if ((pz + chunk->m_offset.z) < height)
					{
						if (pz < Chunk::z_length - 1 && chunk->m_data[loc + Chunk::y_length * Chunk::x_length] > 0)
							chunk->m_data[loc] = 3;
						else if ((pz + chunk->m_offset.z) < 48)
							chunk->m_data[loc] = 1;
						else
							chunk->m_data[loc] = 4; // change to snow
					}
					else if ((pz + chunk->m_offset.z) < 3)
						chunk->m_data[loc] = 2;
					else
						chunk->m_data[loc] = 0;
				}
			}
		}
	}

	void loadChunk(int px, int py, int pz)
	{
		Chunk* chunk = new Chunk(glm::vec3(px * Chunk::x_length, py * Chunk::y_length, pz * Chunk::z_length),
								 pz * Chunk::y_length * Chunk::x_length + py * Chunk::x_length + px);

		generateTerrain(chunk);
		chunk->updateMesh();

		chunks.emplace_back(chunk);
	}
	void unloadChunk(int x, int y, int z)
	{
		int id = z * Chunk::y_length * Chunk::x_length + y * Chunk::x_length + x;
		for (unsigned int i = 0; i < chunks.size(); ++i)
		{
			if (chunks[i]->m_ID == id)
			{
				Chunk* chunk = chunks[i];
				chunks.erase(chunks.begin() + i);
				delete(chunk);
			}
		}
	}

	void draw()
	{
		terrain_shader->use();
		camera->uniformView(terrain_shader);

		unsigned int model_loc = terrain_shader->uniformLoc("model");

		for (unsigned int i = 0; i < chunks.size(); ++i)
		{
			if (chunks[i]->is_empty)
				continue;
			glm::mat4 model(1.0f);
			model = glm::translate(model, chunks[i]->m_offset);
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunks[i]->m_data_buffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, chunks[i]->m_data_buffer);
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			glBindVertexArray(chunks[i]->m_VAO);
			glDrawElements(GL_TRIANGLES, chunks[i]->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
};