#pragma once

#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "../stb_image.h"

class Chunk
{
public:
	// chunk dimensions
	static const unsigned int x_length = 32;
	static const unsigned int y_length = 32;
	static const unsigned int z_length = 32;

	// size of the chunk
	static const unsigned int length = x_length * y_length * z_length;

	// max number of vertices in chunk
	static const unsigned int vertex_size = 9;
	static const unsigned int vertex_length = x_length * y_length * z_length * 24 * vertex_size;

	// reference vertex buffer
	static float vertices[vertex_length];

	static unsigned int VBO;

	static void createTextureArray(const std::string* file_names, const unsigned int length, Shader* shader)
	{
		glActiveTexture(GL_TEXTURE1);

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY, texture);

		const unsigned int data_size = 64 * 48 * 4 * 10;
		unsigned char data[data_size];

		std::cout << "data size: " << data_size << std::endl;

		for (unsigned int i = 0; i < length; ++i)
		{
			int width, height, n_channels;
			unsigned char* tex_data = stbi_load(file_names[i].c_str(), &width, &height, &n_channels, 0);
			std::cout << "data " << i << ": " << (width * height * n_channels) << std::endl;
			if (tex_data)
			{
				unsigned int offset = i * 64 * 48 * 4;
				for (unsigned int j = 0; j < width * height * n_channels; ++j)
				{
					data[offset + j] = tex_data[j];
				}
			}
			else
			{
				std::cout << "Failed to load texture: " << file_names[i] << std::endl;
			}

			stbi_image_free(tex_data);
		}

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB_ALPHA, 64, 48, length, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		// generate mipmap
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 2);

		unsigned int texture_loc = shader->uniformLoc("uTextureArray");
		glUniform1i(texture_loc, 1);
	}

	static void createVertexBuffer()
	{
		unsigned int index = 0;
		for (unsigned int pz = 0; pz < z_length; ++pz)
		{
			for (unsigned int py = 0; py < y_length; ++py)
			{
				for (unsigned int px = 0; px < x_length; ++px)
				{
					float block_location = pz * y_length * x_length + py * x_length + px;
					float block_vertices[] = {
						// u: 0.0f, 0.25f, 0.5f, 0.75f, 1.0f
						// v: 0.0f, 0.3333f, 0.6667f, 1.0f
						// bottom
						// position                       normal              albedo tex
						0.0f + px, 0.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.25f, 1.0f, block_location,
						1.0f + px, 0.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.5f,  1.0f, block_location,
						1.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.5f,  0.6666f, block_location,
						0.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.25f, 0.6666f, block_location,
														  
						// top							  
						0.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.25f, 0.0f, block_location,
						1.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.5f,  0.0f, block_location,
						1.0f + px, 0.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.5f,  0.3334f, block_location,
						0.0f + px, 0.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.25f, 0.3334f, block_location,

						// left
						0.0f + px, 1.0f + py, 1.0f + pz, -1.0f,  0.0f,  0.0f, 0.0f,  0.3334f, block_location,
						0.0f + px, 0.0f + py, 1.0f + pz, -1.0f,  0.0f,  0.0f, 0.25f, 0.3334f, block_location,
						0.0f + px, 0.0f + py, 0.0f + pz, -1.0f,  0.0f,  0.0f, 0.25f, 0.6666f, block_location,
						0.0f + px, 1.0f + py, 0.0f + pz, -1.0f,  0.0f,  0.0f, 0.0f,  0.6666f, block_location,

						// right
						1.0f + px, 1.0f + py, 0.0f + pz,  1.0f,  0.0f,  0.0f, 0.75f, 0.6666f, block_location,
						1.0f + px, 0.0f + py, 0.0f + pz,  1.0f,  0.0f,  0.0f, 0.5f,  0.6666f, block_location,
						1.0f + px, 0.0f + py, 1.0f + pz,  1.0f,  0.0f,  0.0f, 0.5f,  0.3334f, block_location,
						1.0f + px, 1.0f + py, 1.0f + pz,  1.0f,  0.0f,  0.0f, 0.75f, 0.3334f, block_location,

						 // front
						0.0f + px, 0.0f + py, 1.0f + pz,  0.0f, -1.0f,  0.0f, 0.25f, 0.3334f, block_location,
						1.0f + px, 0.0f + py, 1.0f + pz,  0.0f, -1.0f,  0.0f, 0.5f,  0.3334f, block_location,
						1.0f + px, 0.0f + py, 0.0f + pz,  0.0f, -1.0f,  0.0f, 0.5f,  0.6666f, block_location,
						0.0f + px, 0.0f + py, 0.0f + pz,  0.0f, -1.0f,  0.0f, 0.25f, 0.6666f, block_location,
														  
						// back							  
						0.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  1.0f,  0.0f, 1.0f,  0.6666f, block_location,
						1.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  1.0f,  0.0f, 0.75f, 0.6666f, block_location,
						1.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  1.0f,  0.0f, 0.75f, 0.3334f, block_location,
						0.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  1.0f,  0.0f, 1.0f,  0.3334f, block_location
					};
					for (unsigned int i = 0; i < vertex_size * 24; ++i)
					{
						vertices[index] = block_vertices[i];
						index++;
					}
				}
			}
		}

		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	}

	const static glm::vec3 view_normals[6];

	int m_ID;

	unsigned int m_data[length];
	bool is_empty;
	
	std::vector<unsigned int> m_indices;

	unsigned int m_VAO;
	unsigned int m_EBO;
	unsigned int num_elements;

	unsigned int m_data_buffer;

	glm::vec3 m_offset; // offset position

	bool side_occlusion[6];
	Chunk* neighbors[6];

	Chunk(glm::vec3 offset, int ID) : m_offset(offset), m_EBO(0), num_elements(0), m_VAO(0), m_ID(ID), is_empty(true)
	{
		for (unsigned int i = 0; i < length; ++i)
		{
			m_data[i] = 0;
		}
		for (unsigned int i = 0; i < 6; ++i)
		{
			side_occlusion[i] = false;
			neighbors[i] = nullptr;
		}
	}
	~Chunk()
	{
		if (!is_empty)
		{
			deleteBufferData();
		}
	}
	/**
	creates data of this chunk on the GPU
	*/
	void createBufferData()
	{
		if (is_empty)
		{
			return;
		}

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glGenBuffers(1, &m_EBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);


		glGenBuffers(1, &m_data_buffer);

		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_data_buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(m_data), &m_data, GL_DYNAMIC_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_data_buffer);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		num_elements = m_indices.size();
		m_indices.clear();
		m_indices.shrink_to_fit();
	}
	/**
	update data of this chunk on the GPU
	*/
	void updateBufferData()
	{
		// reset the element buffer
	}
	/**
	removes all data of this chunk from the GPU
	*/
	void deleteBufferData()
	{
		// delete data buffer
		glDeleteBuffers(1, &m_data_buffer);
		// delete EBO
		glDeleteBuffers(1, &m_EBO);
		// delete VAO
		glDeleteVertexArrays(1, &m_VAO);
	}
	void updateMesh()
	{
		is_empty = false;
		// count index buffer size
		unsigned int index_buffer_size = 0;
		for (int pz = 0; pz < z_length; ++pz)
		{
			for (int py = 0; py < y_length; ++py)
			{
				for (int px = 0; px < x_length; ++px)
				{
					if (isEmpty(px, py, px))
						continue;
					if (isEmpty(px + 1, py, pz))
						index_buffer_size++;
					if (isEmpty(px - 1, py, pz))
						index_buffer_size++;
					if (isEmpty(px, py + 1, pz))
						index_buffer_size++;
					if (isEmpty(px, py - 1, pz))
						index_buffer_size++;
					if (isEmpty(px, py, pz + 1))
						index_buffer_size++;
					if (isEmpty(px, py, pz - 1))
						index_buffer_size++;
				}
			}
		}
		index_buffer_size *= 6;
		num_elements = index_buffer_size;
		//std::cout << "counted elements: " << num_elements << std::endl;

		// update index buffer
		m_indices.clear();
		m_indices.shrink_to_fit();
		m_indices.reserve(index_buffer_size);

		for (int pz = 0; pz < z_length; ++pz)
		{
			for (int py = 0; py < y_length; ++py)
			{
				for (int px = 0; px < x_length; ++px)
				{
					if (isEmpty(px, py, pz))
						continue;
					// right
					if (isEmpty(px + 1, py, pz))
						addFace(px, py, pz, 3);
					// left
					if (isEmpty(px - 1, py, pz))
						addFace(px, py, pz, 2);
					// back
					if (isEmpty(px, py + 1, pz))
						addFace(px, py, pz, 5);
					// front
					if (isEmpty(px, py - 1, pz))
						addFace(px, py, pz, 4);
					// top
					if (isEmpty(px, py, pz + 1))
						addFace(px, py, pz, 1);
					// bottom
					if (isEmpty(px, py, pz - 1))
						addFace(px, py, pz, 0);
				}
			}
		}

		//std::cout << "added elements: " << m_indices.size() << std::endl;

		if (m_indices.size() == 0)
		{
			is_empty = true;

			num_elements = m_indices.size();
			m_indices.clear();
			m_indices.shrink_to_fit();
		}
	}
private:
	struct FaceIndex
	{
		unsigned int indices[6];

		void addIndices(std::vector<unsigned int> index_buffer)
		{
			index_buffer.insert(index_buffer.end(), indices, indices + 6);
		}
	};
	bool isEmpty(int x, int y, int z) const
	{
		if (x < 0 || x > x_length - 1)
			return true;
		if (y < 0 || y > y_length - 1)
			return true;
		if (z < 0 || z > z_length - 1)
			return true;

		return m_data[z * y_length * x_length + y * x_length + x] == 0;
	}
	void addFace(unsigned int x, unsigned int y, unsigned int z, unsigned int face_index)
	{
		unsigned int offset = z * x_length * y_length + y * x_length + x;
		offset *= 24;
		offset += face_index * 4;
		m_indices.insert(m_indices.end(), {offset, offset + 1, offset + 2,
										   offset + 2, offset + 3, offset});
	}
};

const unsigned int Chunk::x_length;
const unsigned int Chunk::y_length;
const unsigned int Chunk::z_length;

const unsigned int Chunk::length;

const unsigned int Chunk::vertex_size;
const unsigned int Chunk::vertex_length;

unsigned int Chunk::VBO;

float Chunk::vertices[Chunk::vertex_length];

const glm::vec3 Chunk::view_normals[] = {
	glm::vec3( 0.0f,  0.0f, -1.0f), // bottom
	glm::vec3( 0.0f,  0.0f,  1.0f), // top
	glm::vec3(-1.0f,  0.0f,  0.0f), // left
	glm::vec3( 1.0f,  0.0f,  0.0f), // right
	glm::vec3( 0.0f, -1.0f,  0.0f), // front
	glm::vec3( 0.0f,  1.0f,  0.0f)  // back
};