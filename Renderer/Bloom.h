#pragma once

#include <iostream>
#include <vector>

#include <glad/glad.h>

#include <glm/glm.hpp>

struct BloomMip
{
	glm::vec2 size;
	glm::ivec2 int_size;
	unsigned int texture;
};

class BloomBuffer
{
public:
	BloomBuffer() : m_init(false), bloom_buffer(0)
	{

	}
	~BloomBuffer()
	{
		destroy();
	}
	bool init(unsigned int window_width, unsigned int window_height, unsigned int mip_chain_length)
	{
		if (m_init)
			return true;

		glGenFramebuffers(1, &bloom_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, bloom_buffer);

		glm::vec2 mip_size((float)window_width, (float)window_height);
		glm::ivec2 mip_int_size((int)window_width, (int)window_height);

		for (unsigned int i = 0; i < mip_chain_length; ++i)
		{
			BloomMip mip;

			mip_size *= 0.5f;
			mip_int_size /= 2;
			mip.size = mip_size;
			mip.int_size = mip_int_size;

			glGenTextures(1, &mip.texture);
			glBindTexture(GL_TEXTURE_2D, mip.texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, (int)mip_size.x, (int)mip_size.y, 0, GL_RGB, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			m_mip_chain.emplace_back(mip);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_mip_chain[0].texture, 0);

		unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, attachments);

		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Bloom Framebuffer not complete! status: " << status << std::endl;
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_init = true;
		return true;
	}
	void destroy()
	{
		for (unsigned int i = 0; i < m_mip_chain.size(); ++i)
		{
			glDeleteTextures(1, &m_mip_chain[i].texture);
			m_mip_chain[i].texture = 0;
		}
		glDeleteFramebuffers(1, &bloom_buffer);
		bloom_buffer = 0;
		m_init = false;
	}
	void bindForWriting()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, bloom_buffer);
	}
	const std::vector<BloomMip>& mipChain() const
	{
		return m_mip_chain;
	}

private:
	bool m_init;
	unsigned int bloom_buffer;
	std::vector<BloomMip> m_mip_chain;
};