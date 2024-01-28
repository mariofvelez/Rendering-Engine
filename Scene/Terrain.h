#pragma once

#include <vector>
#include <stdlib.h>
#include <algorithm>

#include "Chunk.h"
#include "Camera.h"
#include "../Renderer/Shader.h"

class Terrain
{
	std::vector<Chunk*> chunks;

	std::vector<Chunk*> chunks_to_load;
	std::vector<Chunk*> chunks_to_unload;

	bool chunks_ready_to_delete;

public:
	Shader* terrain_shader;
	Camera* camera;

	Terrain(Camera* camera) : camera(camera), chunks_ready_to_delete(false)
	{
		chunks.reserve(1000);

		chunks_to_load.reserve(100);
		chunks_to_unload.reserve(100);

		terrain_shader = new Shader("Shaders/Vertex.shader", "Shaders/Fragment.shader");
		terrain_shader->use();

		camera->uniformView(terrain_shader);

		camera->updateProjection(9.0f / 6.0f);
		camera->uniformProjection(terrain_shader);

		for (unsigned int i = 0; i < 256; ++i)
			p[256 + i] = p[i];
	}
	~Terrain()
	{
		delete(terrain_shader);
	}
	int p[512] = { 151,160,137,91,90,15,
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
	inline float fade(float t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	inline float lerp(float t, float a, float b) { return a + t * (b - a); }
	inline float grad2D(int hash, float x, float y)
	{
		int h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
		float u = h < 8 ? x : y,                 // INTO 12 GRADIENT DIRECTIONS.
			v = h < 4 ? y : h == 12 || h == 14 ? x : 0;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
	float noise2D(float x, float y)
	{
		int X = (int)floor(x) & 255,                  // FIND UNIT CUBE THAT
			Y = (int)floor(y) & 255;                  // CONTAINS POINT.
		x -= floor(x);                           // FIND RELATIVE X,Y,Z
		y -= floor(y);                                // OF POINT IN CUBE.
		float u = fade(x),                                // COMPUTE FADE CURVES
			v = fade(y);                                // FOR EACH OF X,Y,Z.
		int A = p[X] + Y, AA = p[A], AB = p[A + 1],      // HASH COORDINATES OF
			B = p[X + 1] + Y, BA = p[B], BB = p[B + 1];      // THE 8 CUBE CORNERS,

		return lerp(v, lerp(u, grad2D(p[AA], x, y),  // AND ADD
			grad2D(p[BA], x - 1, y)), // BLENDED
			lerp(u, grad2D(p[AB], x, y - 1),  // RESULTS
				grad2D(p[BB], x - 1, y - 1)));// FROM  8
	}
	float grad3D(int hash, float x, float y, float z) {
		int h = hash & 15;
		float u = h < 8 ? x : y,
			v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}
	float noise3D(float x, float y, float z) {
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

		return lerp(w, lerp(v, lerp(u, grad3D(p[AA], x, y, z),
			grad3D(p[BA], x - 1, y, z)),
			lerp(u, grad3D(p[AB], x, y - 1, z),
				grad3D(p[BB], x - 1, y - 1, z))),
			lerp(v, lerp(u, grad3D(p[AA + 1], x, y, z - 1),
				grad3D(p[BA + 1], x - 1, y, z - 1)),
				lerp(u, grad3D(p[AB + 1], x, y - 1, z - 1),
					grad3D(p[BB + 1], x - 1, y - 1, z - 1))));
	}
	inline float perlin2D(float x, float y, float period, float scale)
	{
		float val = (noise2D(x / period, y / period) + 1.0f) / 2.0f;
		return val * scale;
	}
	inline float perlin3D(float x, float y, float z, float period, float scale)
	{
		float val = (noise3D(x / period, y / period, z / period) + 1.0f) / 2.0f;
		return val * scale;
	}
	void generateTerrain(Chunk* chunk)
	{
		const unsigned int grass = 1;
		const unsigned int sand = 2;
		const unsigned int stone = 3;
		const unsigned int snow = 4;
		const unsigned int mud = 5;
		const unsigned int magma = 6;
		const unsigned int darkstone = 7;
		const unsigned int log = 8;
		const unsigned int shallow_water = 9;
		const unsigned int deep_water = 10;

		const float cave_mask_min = 0.2f;
		const float cave_mask_max = 0.6f;

		srand(chunk->m_ID);

		for (int py = 0; py < Chunk::y_length; ++py)
		{
			float y = (float) py + chunk->m_offset.y;
			for (int px = 0; px < Chunk::x_length; ++px)
			{
				float x = (float) px + chunk->m_offset.x;

				float height = perlin2D(x + 27.3f, y + 37.7f, 256.0f, 128);
				height += perlin2D(x + 783.4f, y + 349.7f, 128.0f, 64);
				height += perlin2D(x + 587.3f, y + 149.9f, 64.0f, 16);
				height += perlin2D(x + 383.7f, y + 421.1f, 16.0f, 8);
				height -= 64.0f;

				float cave_mask = perlin2D(x + 6.7, y + 8.3, 128, 1.0f);

				bool is_log = rand() % 100 == 0 && height > 32.0f && height < 48.0f;
				float log_bottom = height;
				float log_top = height + 4.0f + (float)(rand() % 5);

				/*float steepness = perlin2D(x, y, 128, 2.0f);
				if (steepness < 1.0f)
					steepness = 1.0f;

				float moisture = perlin2D(x, y, 64, 1.0f);
				moisture += perlin2D(x + 6.7, y + 8.3, 26, 0.5f);*/

				for (int pz = 0; pz < Chunk::z_length; pz++)
				{
					float z = (float) pz + chunk->m_offset.z;
					if (z - 5 > height && z > 28.0f)
						break;

					int loc = pz * Chunk::y_length * Chunk::x_length + py * Chunk::x_length + px;
					if (z < 10)
					{
						chunk->m_data[loc] = stone;
						continue;
					}

					//float cave = 100;
					float cave = perlin3D(x, y, z, 32, 16.0f);
					cave += perlin3D(x + 17.3f, y + 55.8f, z + 29.5f, 8, 4.0f);

					/*offsetx += perlin3D(x + 17.3f, y + 55.8f, z + 29.5f, 8, 4.0f);
					offsetx += x;
					offsety += perlin3D(x + 11.9f, y + 45.7f, z + 123.1f, 8, 4.0f);
					offsety += y;
					offsetz += perlin3D(x + 231.7f, y + 57.7f, z + 41.9f, 8, 2.0f);
					offsetz += z + 16;

					float height = perlin2D(offsetx + 27.3f, offsety + 37.7f, 256, 128) * steepness;
					height += perlin2D(offsetx + 783.4f, offsety + 349.7f, 128, 64) * steepness;
					height += perlin2D(offsetx + 587.3f, offsety + 149.9f, 64, 16) * steepness;
					height += perlin2D(offsetx + 383.7f, offsety + 421.1f, 16, 16) * steepness;
					height -= 32.0f;*/

					/*if (offsetz < height)
					{
						if (offsetz + 3 > height)
						{
							if (moisture > 0.9f && offsetz < 48)
								chunk->m_data[loc] = mud;
							else if (offsetz < 64)
								chunk->m_data[loc] = grass;
							else if (offsetz > 100)
								chunk->m_data[loc] = snow;
						}
						else
						{
							if (offsetz < 32 + (rand() % 8))
								chunk->m_data[loc] = darkstone;
							else
								chunk->m_data[loc] = stone;
						}
					}*/
					if (z < 28.0f)
					{
						if (height < 20.0f)
							chunk->m_data[loc] = deep_water;
						else
							chunk->m_data[loc] = shallow_water;
					}
					else
						chunk->m_data[loc] = 0;
						

					float cave_thresh = 0.0f;
					if (cave_mask > cave_mask_max)
						cave_thresh = 8.0f;
					else if (cave_mask > cave_mask_min)
					{
						float t = cave_mask - cave_mask_min;
						t *= cave_mask_max - cave_mask_min;
						cave_thresh = lerp(t, 0.0f, 8.0f);
					}
						
					if (z < height && cave > cave_thresh)
					{
						if (z + 3 > height && z < 32.0f)
							chunk->m_data[loc] = sand;
						else if (z + 1 > height && z < 48.0f)
							chunk->m_data[loc] = grass;
						else if (z + 1 > height && z > 64.0f)
							chunk->m_data[loc] = snow;
						else
							chunk->m_data[loc] = stone;
					}
					else if (is_log && cave > cave_thresh && z >= log_bottom && z <= log_top)
					{
						chunk->m_data[loc] = log;
					}
					
				}
			}
		}
	}
	int getChunkID(int x, int y, int z)
	{
		return (z - 64) * 128 * 128 + (y - 64) * 128 + (x - 64);
	}
	void loadChunk(int px, int py, int pz)
	{
		Chunk* chunk = new Chunk(glm::vec3((float)px * Chunk::x_length,
			(float)py * Chunk::y_length,
			(float)pz * Chunk::z_length),
			getChunkID(px, py, pz));

		generateTerrain(chunk);
		chunk->updateMesh();
		//chunk->createBufferData();

		chunks_to_load.emplace_back(chunk);
	}
	void unloadChunk(int x, int y, int z)
	{
		int id = getChunkID(x, y, z);
		unloadChunk(id);
	}
	void unloadChunk(int id)
	{
		for (unsigned int i = 0; i < chunks.size(); ++i)
		{
			if (chunks[i]->m_ID == id)
			{
				Chunk* chunk = chunks[i];

				unloadChunk(chunk);
				//chunks.erase(chunks.begin() + i);
				//i--;
				//delete(chunk);
			}
		}
	}
	void unloadChunk(Chunk* chunk)
	{
		bool chunk_found = false;
		for (unsigned int i = 0; i < chunks_to_unload.size(); ++i)
			if (chunks_to_unload[i]->m_ID == chunk->m_ID)
				chunk_found = true;

		if (!chunk_found)
			chunks_to_unload.emplace_back(chunk);
	}
	bool isChunkLoaded(int x, int y, int z)
	{
		int id = getChunkID(x, y, z);
		for (unsigned int i = 0; i < chunks.size(); ++i)
			if (chunks[i]->m_ID == id)
				return true;
		return false;
	}
	void updateLoadedChunks(bool* running)
	{
		int x_coord = (int)floor(camera->m_pos.x / Chunk::x_length);
		int y_coord = (int)floor(camera->m_pos.y / Chunk::y_length);
		int z_coord = (int)floor(camera->m_pos.z / Chunk::z_length);

		if (!chunks_ready_to_delete)
		{
			for (unsigned int i = 0; i < chunks.size(); ++i)
			{
				glm::vec3 chunk_center = chunks[i]->m_offset + glm::vec3(16.0f, 16.0f, 16.0f); // change to chunk::x_length / 2.0f etc
				float dist = glm::distance(chunk_center, camera->m_pos);
				if (dist > 7 * 32)
				{
					unloadChunk(chunks[i]);
				}
			}
			chunks_ready_to_delete = true;
		}
		
		std::vector<Chunk*> chunks_to_generate;
		for (int z = z_coord - 2; z <= z_coord + 2; ++z)
		{
			for (int y = y_coord - 4; y <= y_coord + 4; ++y)
			{
				for (int x = x_coord - 4; x <= x_coord + 4; ++x)
				{
					if (!isChunkLoaded(x, y, z))
					{
						Chunk* chunk = new Chunk(glm::vec3((float)x * Chunk::x_length,
														   (float)y * Chunk::y_length,
														   (float)z * Chunk::z_length),
												 getChunkID(x, y, z));

						chunks_to_generate.emplace_back(chunk);
					}
				}
			}
		}

		std::sort(chunks_to_generate.begin(), chunks_to_generate.end(), [this](Chunk* a, Chunk* b) {
			glm::vec3 a_pos = glm::vec3(a->m_offset.x, a->m_offset.y, a->m_offset.z * 2.0f);
			glm::vec3 b_pos = glm::vec3(b->m_offset.x, b->m_offset.y, b->m_offset.z * 2.0f);
			return glm::distance(camera->m_pos, a_pos) < glm::distance(camera->m_pos, b_pos);
		});

		for (unsigned int i = 0; i < chunks_to_generate.size(); ++i)
		{
			if (*running)
			{
				//std::cout << "generating chunk | data: ";
				float t1 = (float) glfwGetTime();
				Chunk* chunk = chunks_to_generate[i];
				generateTerrain(chunk);
				float t2 = glfwGetTime();
				chunk->updateMesh();
				float t3 = glfwGetTime();
				chunks_to_load.emplace_back(chunk);
				//std::cout << (t2 - t1) << "s | mesh: " << (t3 - t2) << "s" << std::endl;
			}
		}
	}
	void updateChunks()
	{
		
		if (chunks_ready_to_delete)
		{
			for (unsigned int i = 0; i < chunks_to_unload.size(); ++i)
			{
				Chunk* chunk = chunks_to_unload[i];
				chunks.erase(std::remove(chunks.begin(), chunks.end(), chunk), chunks.end());
			}
			while (chunks_to_unload.size() > 0)
			{
				Chunk* chunk = chunks_to_unload[0];
				chunks_to_unload.erase(chunks_to_unload.begin());
				delete(chunk);
			}
			chunks_to_unload.clear();
			chunks_ready_to_delete = false;
		}

		for (unsigned int i = 0; i < chunks_to_load.size(); ++i)
		{
			Chunk* chunk = chunks_to_load[i];
			chunk->createBufferData();
			chunks.emplace_back(chunk);
		}
		chunks_to_load.clear();
	}

	void draw()
	{
		updateChunks();

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		terrain_shader->use();
		camera->uniformView(terrain_shader);

		unsigned int model_loc = terrain_shader->uniformLoc("model");

		glm::mat4 cam_transform = camera->projection * camera->view;

		for (unsigned int i = 0; i < chunks.size(); ++i)
		{
			if (chunks[i]->is_empty)
				continue;
			glm::vec3 center = chunks[i]->m_offset + glm::vec3(16.0f, 16.0f, 16.0f); // change to chunk::x_length / 2.0f etc
			float dist = glm::distance(center, camera->m_pos);
			if (dist > 5 * 32)
				continue;

			// chunk culling
			glm::vec4 chunk_pos = glm::vec4(center.x, center.y, center.z, 1.0f);
			glm::vec4 pos = cam_transform * chunk_pos;
			if (pos.z < -24)
				continue;

			//std::cout << "pos: " << pos.x << ", " << pos.y << ", " << pos.z << std::endl;

			glm::mat4 model(1.0f);
			model = glm::translate(model, chunks[i]->m_offset);
			glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

			glBindBuffer(GL_SHADER_STORAGE_BUFFER, chunks[i]->m_data_buffer);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, chunks[i]->m_data_buffer);
			//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

			glBindVertexArray(chunks[i]->m_VAO);
			glDrawElements(GL_TRIANGLES, chunks[i]->num_elements, GL_UNSIGNED_INT, 0);
		}
	}
};