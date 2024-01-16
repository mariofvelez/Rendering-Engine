#pragma once

#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>

class Chunk
{
public:
	// chunk dimensions
	static const unsigned int x_length = 16;
	static const unsigned int y_length = 16;
	static const unsigned int z_length = 16;

	// size of the chunk
	static const unsigned int length = x_length * y_length * z_length;

	// max number of vertices in chunk
	static const unsigned int vertex_size = 8;
	static const unsigned int vertex_length = x_length * y_length * z_length * 24 * vertex_size;

	// reference vertex buffer
	static float vertices[vertex_length];

	static unsigned int VBO;

	static void createVertexBuffer()
	{
		unsigned int index = 0;
		for (unsigned int pz = 0; pz < z_length; ++pz)
		{
			for (unsigned int py = 0; py < y_length; ++py)
			{
				for (unsigned int px = 0; px < x_length; ++px)
				{
					float block_vertices[] = {
						// u: 0.0f, 0.25f, 0.5f, 0.75f, 1.0f
						// v: 0.0f, 0.3333f, 0.6667f, 1.0f
						// bottom
						0.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.25f, 0.6667f,
						1.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.5f,  0.6667f,
						1.0f + px, 0.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.5f,  1.0f,
						0.0f + px, 0.0f + py, 0.0f + pz,  0.0f,  0.0f, -1.0f, 0.25f, 1.0f,
														  
						// top							  
						0.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.25f, 0.0f,
						1.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.5f,  0.0f,
						1.0f + px, 0.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.5f,  0.3333f,
						0.0f + px, 0.0f + py, 1.0f + pz,  0.0f,  0.0f,  1.0f, 0.25f, 0.3333f,

						// left
						0.0f + px, 1.0f + py, 1.0f + pz, -1.0f,  0.0f,  0.0f, 0.0f,  0.3333f,
						0.0f + px, 0.0f + py, 1.0f + pz, -1.0f,  0.0f,  0.0f, 0.25f, 0.3333f,
						0.0f + px, 0.0f + py, 0.0f + pz, -1.0f,  0.0f,  0.0f, 0.25f, 0.6667f,
						0.0f + px, 1.0f + py, 0.0f + pz, -1.0f,  0.0f,  0.0f, 0.0f,  0.6667f,

						// right
						1.0f + px, 1.0f + py, 1.0f + pz,  1.0f,  0.0f,  0.0f, 0.75f, 0.3333f,
						1.0f + px, 0.0f + py, 1.0f + pz,  1.0f,  0.0f,  0.0f, 0.5f,  0.3333f,
						1.0f + px, 0.0f + py, 0.0f + pz,  1.0f,  0.0f,  0.0f, 0.5f,  0.6667f,
						1.0f + px, 1.0f + py, 0.0f + pz,  1.0f,  0.0f,  0.0f, 0.75f, 0.6667f,

						 // front
						0.0f + px, 0.0f + py, 1.0f + pz,  0.0f, -1.0f,  0.0f, 0.25f, 0.3333f,
						1.0f + px, 0.0f + py, 1.0f + pz,  0.0f, -1.0f,  0.0f, 0.5f,  0.3333f,
						1.0f + px, 0.0f + py, 0.0f + pz,  0.0f, -1.0f,  0.0f, 0.5f,  0.6667f,
						0.0f + px, 0.0f + py, 0.0f + pz,  0.0f, -1.0f,  0.0f, 0.25f, 0.6667f,
														  
						// back							  
						0.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  1.0f,  0.0f, 1.0f,  0.3333f,
						1.0f + px, 1.0f + py, 1.0f + pz,  0.0f,  1.0f,  0.0f, 0.75f, 0.3333f,
						1.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  1.0f,  0.0f, 0.75f, 0.6667f,
						0.0f + px, 1.0f + py, 0.0f + pz,  0.0f,  1.0f,  0.0f, 1.0f,  0.6667f
					};
					for (unsigned int i = 0; i < 8 * 24; ++i)
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

	unsigned int m_data[length];
	
	std::vector<unsigned int> m_indices;

	unsigned int m_VAO;
	unsigned int m_EBO;
	unsigned int num_elements;

	glm::vec3 m_offset; // offset position

	Chunk() : m_offset(0.0f, 0.0f, 0.0f), m_EBO(0), num_elements(0), m_VAO(0)
	{
		for (unsigned int i = 0; i < length; ++i)
		{
			m_data[i] = 0;
		}
	}
	~Chunk() {}

	void updateMesh()
	{
		// count index buffer size
		unsigned int index_buffer_size = 0;
		for (unsigned int pz = 0; pz < z_length; ++pz)
		{
			for (unsigned int py = 0; py < y_length; ++py)
			{
				for (unsigned int px = 0; px < x_length; ++px)
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

		// update index buffer
		m_indices.clear();
		m_indices.shrink_to_fit();
		m_indices.reserve(index_buffer_size);

		for (unsigned int pz = 0; pz < z_length; ++pz)
		{
			for (unsigned int py = 0; py < y_length; ++py)
			{
				for (unsigned int px = 0; px < x_length; ++px)
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

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &m_EBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
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
	bool isEmpty(unsigned int x, unsigned int y, unsigned int z) const
	{
		if (x < 0 || x > x_length - 1)
			return true;
		if (y < 0 || y > y_length - 1)
			return true;
		if (z < 0 || z > z_length - 1)
			return true;

		return m_data[z * y_length * x_length + y * x_length + x] == 0;
	}
	inline unsigned int getVertexIndex(unsigned int vx, unsigned int vy, unsigned int vz)
	{
		return vz * (y_length + 1) * (x_length + 1) + vy * (x_length + 1) + vx;
	}
	void addFace(unsigned int x, unsigned int y, unsigned int z, unsigned int face_index)
	{
		unsigned int offset = z * x_length * y_length + y * x_length + x;
		offset *= 24;
		offset += face_index * 4;
		m_indices.insert(m_indices.end(), {offset, offset + 1, offset + 2,
										   offset + 2, offset + 3, offset});
	}
	FaceIndex getTopIndices(unsigned int x, unsigned int y, unsigned int z)
	{
		FaceIndex face;
		face.indices[0] = getVertexIndex(x,     y,     z + 1);
		face.indices[1] = getVertexIndex(x,     y + 1, z + 1);
		face.indices[2] = getVertexIndex(x + 1, y,     z + 1);
		face.indices[3] = getVertexIndex(x,     y + 1, z + 1);
		face.indices[4] = getVertexIndex(x + 1, y + 1, z + 1);
		face.indices[5] = getVertexIndex(x + 1, y,     z + 1);
		return face;
	}
	FaceIndex getBottomIndices(unsigned int x, unsigned int y, unsigned int z)
	{
		FaceIndex face;
		face.indices[0] = getVertexIndex(x,     y,     z);
		face.indices[1] = getVertexIndex(x + 1, y,     z);
		face.indices[2] = getVertexIndex(x,     y + 1, z);
		face.indices[3] = getVertexIndex(x,     y + 1, z);
		face.indices[4] = getVertexIndex(x + 1, y,     z);
		face.indices[5] = getVertexIndex(x + 1, y + 1, z);
		return face;
	}
	FaceIndex getBackVertices(unsigned int x, unsigned int y, unsigned int z)
	{
		FaceIndex face;
		face.indices[0] = getVertexIndex(x,     y, z + 1);
		face.indices[1] = getVertexIndex(x + 1, y, z + 1);
		face.indices[2] = getVertexIndex(x,     y, z);
		face.indices[3] = getVertexIndex(x + 1, y, z + 1);
		face.indices[4] = getVertexIndex(x + 1, y, z);
		face.indices[5] = getVertexIndex(x,     y, z);
		return face;
	}
	FaceIndex getFrontVertices(unsigned int x, unsigned int y, unsigned int z)
	{
		FaceIndex face;
		face.indices[0] = getVertexIndex(x,     y + 1, z + 1);
		face.indices[1] = getVertexIndex(x,     y + 1, z);
		face.indices[2] = getVertexIndex(x + 1, y + 1, z + 1);
		face.indices[3] = getVertexIndex(x + 1, y + 1, z + 1);
		face.indices[4] = getVertexIndex(x,     y + 1, z);
		face.indices[5] = getVertexIndex(x + 1, y + 1, z);
		return face;
	}
	FaceIndex getLeftVertices(unsigned int x, unsigned int y, unsigned int z)
	{
		FaceIndex face;
		face.indices[0] = getVertexIndex(x, y,     z);
		face.indices[1] = getVertexIndex(x, y + 1, z + 1);
		face.indices[2] = getVertexIndex(x, y,     z + 1);
		face.indices[3] = getVertexIndex(x, y,     z);
		face.indices[4] = getVertexIndex(x, y + 1, z);
		face.indices[5] = getVertexIndex(x, y + 1, z + 1);
		return face;
	}
	FaceIndex getRightVertices(unsigned int x, unsigned int y, unsigned int z)
	{
		FaceIndex face;
		face.indices[0] = getVertexIndex(x + 1, y,     z);
		face.indices[1] = getVertexIndex(x + 1, y,     z + 1);
		face.indices[2] = getVertexIndex(x + 1, y + 1, z + 1);
		face.indices[3] = getVertexIndex(x + 1, y,     z);
		face.indices[4] = getVertexIndex(x + 1, y + 1, z + 1);
		face.indices[5] = getVertexIndex(x + 1, y + 1, z);
		return face;
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