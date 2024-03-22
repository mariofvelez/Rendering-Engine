#pragma once

#include <random>

#include "Shader.h"
#include "Bloom.h"

#include "../Scene/World.h"

// texture binding locations
#define G_POSITION_TEXTURE 2
#define G_NORMAL_TEXTURE 3
#define G_ALBEDO_SPEC_TEXTURE 4
#define SSAO_NOISE_TEXTURE 5
#define SSAO_BLUR_INPUT_TEXTURE 5
#define SSAO_TEXTURE 5
#define HDR_COLOR_TEXTURE 5
#define BLOOM_COLOR_TEXTURE 6

class Renderer
{
public:
	Renderer(int screen_width, int screen_height) : screen_width(screen_width), screen_height(screen_height)
	{
		initQuad();

		initGBuffer(screen_width, screen_height);

		initLightingBuffer(screen_width, screen_height);

		initSSAO(screen_width, screen_height);

		initHDRShader();

		initBloom(screen_width, screen_height);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, g_position);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, g_normal);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, g_color_spec);

		glEnable(GL_DEPTH_TEST);
	}
	~Renderer()
	{
		delete(ssao_shader);
		delete(ssao_blur_shader);
		delete(lighting_shader);
		delete(bloom_downsample_shader);
		delete(bloom_upsample_shader);
		delete(hdr_shader);
	}
	void render(World* world)
	{
		// clear screen
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// draw world to G buffer
		renderWorld(world);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// --- SHADING PASSES ---
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		// SSAO
		glBindFramebuffer(GL_FRAMEBUFFER, ssao_buffer);
		glClear(GL_COLOR_BUFFER_BIT);
		ssao_shader->use();
		world->camera->uniformProjection(ssao_shader);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, noise_texture);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		
		// SSAO blur
		glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_buffer);
		glClear(GL_COLOR_BUFFER_BIT);
		ssao_blur_shader->use();
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ssao_color);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// shadow mapping
		glViewport(0, 0, screen_width, screen_height);

		// lighting pass
		glBindFramebuffer(GL_FRAMEBUFFER, lighting_buffer);
		glClear(GL_COLOR_BUFFER_BIT);
		lighting_shader->use();
		world->light->uniformShader(lighting_shader, &world->camera->view, "dirlight");

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, ssao_blur);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		// --- POST PROCESS PASSES ---

		// bloom
		bloom_buffer.bindForWriting();

		const std::vector<BloomMip>& mip_chain = bloom_buffer.mipChain();

		// bloom downsample
		bloom_downsample_shader->use();
		bloom_downsample_shader->setVec2("srcResolution", src_viewport_size_float);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdr_color);

		bloom_downsample_shader->setVec3("threshold", 1.0f, 1.0f, 1.0f);

		for (unsigned int i = 0; i < mip_chain.size(); ++i)
		{
			const BloomMip& mip = mip_chain[i];
			glViewport(0, 0, mip.size.x, mip.size.y);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mip.texture, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			bloom_downsample_shader->setVec2("srcResolution", mip.size);
			bloom_downsample_shader->setVec3("threshold", 0.0f, 0.0f, 0.0f);

			glBindTexture(GL_TEXTURE_2D, mip.texture);
		}

		// bloom upsample
		bloom_upsample_shader->use();
		bloom_upsample_shader->setFloat("filterRadius", bloom_filter_radius);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glBlendEquation(GL_FUNC_ADD);

		for (int i = mip_chain.size() - 1; i > 0; --i)
		{
			const BloomMip& mip = mip_chain[i];
			const BloomMip& next_mip = mip_chain[i - 1];

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, mip.texture);

			glViewport(0, 0, next_mip.size.x, next_mip.size.y);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, next_mip.texture, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glDisable(GL_BLEND);

		glViewport(0, 0, screen_width, screen_height);

		// HDR and gamma correction
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		hdr_shader->use();

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, hdr_color);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mip_chain[0].texture);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
	}

private:
	int screen_width, screen_height;

	unsigned int quadVAO;

	// g buffer
	unsigned int g_buffer;
	unsigned int g_position, g_normal, g_color_spec;

	// SSAO
	unsigned int noise_texture;
	unsigned int ssao_buffer;
	unsigned int ssao_color;

	Shader* ssao_shader;

	unsigned int ssao_blur_buffer;
	unsigned int ssao_blur;

	Shader* ssao_blur_shader;

	// lighting buffer
	unsigned int lighting_buffer;
	unsigned int hdr_color;

	Shader* lighting_shader;

	// bloom
	BloomBuffer bloom_buffer;
	glm::vec2 src_viewport_size_float;

	Shader* bloom_downsample_shader;
	Shader* bloom_upsample_shader;

	const unsigned int num_bloom_mips = 8;
	const float bloom_filter_radius = 0.005f; // experiment with this value

	// HDR
	Shader* hdr_shader;

	float lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}

	void initQuad()
	{
		float quad_vertices[] = {
			// positions   // texCoords
			-1.0f,  1.0f,  0.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};

		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);

		unsigned int quadVBO;
		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void initGBuffer(int screen_width, int screen_height)
	{
		glGenFramebuffers(1, &g_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, g_buffer);

		// position buffer
		glGenTextures(1, &g_position);
		glBindTexture(GL_TEXTURE_2D, g_position);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position, 0);

		// normal buffer
		glGenTextures(1, &g_normal);
		glBindTexture(GL_TEXTURE_2D, g_normal);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal, 0);

		// color and specular buffer
		glGenTextures(1, &g_color_spec);
		glBindTexture(GL_TEXTURE_2D, g_color_spec);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width, screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_color_spec, 0);

		// attachments for rendering
		unsigned int attachments[3] = {
			GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2
		};
		glDrawBuffers(3, attachments);

		// depth buffer
		unsigned int rbo_depth;
		glGenRenderbuffers(1, &rbo_depth);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);

		// check for completeness
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "G buffer is not complete!" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void initLightingBuffer(int screen_width, int screen_height)
	{
		glGenFramebuffers(1, &lighting_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, lighting_buffer);

		glGenTextures(1, &hdr_color);
		glBindTexture(GL_TEXTURE_2D, hdr_color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width, screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_color, 0);

		lighting_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/LightingFragment.shader");
		lighting_shader->use();
		lighting_shader->setInt("gPosition", G_POSITION_TEXTURE);
		lighting_shader->setInt("gNormal", G_NORMAL_TEXTURE);
		lighting_shader->setInt("gAlbedoSpec", G_ALBEDO_SPEC_TEXTURE);
		lighting_shader->setInt("ssao", SSAO_TEXTURE);
		// TODO - set directional light uniform values for this shader
	}

	void initSSAO(int screen_width, int screen_height)
	{
		// noise texture
		std::uniform_real_distribution<float> rand_floats(0.0f, 1.0f);
		std::default_random_engine generator;
		std::vector<glm::vec3> ssao_kernel;
		for (unsigned int i = 0; i < 16; ++i)
		{
			glm::vec3 sample(
				rand_floats(generator) * 2.0f - 1.0f,
				rand_floats(generator) * 2.0f - 1.0f,
				rand_floats(generator)
			);
			sample = glm::normalize(sample);
			sample *= rand_floats(generator);
			float scale = (float)i / 16.0f;
			scale = lerp(0.1f, 1.0f, scale * scale);
			sample *= scale;
			ssao_kernel.push_back(sample);
		}

		std::vector<glm::vec3> ssao_noise;
		for (unsigned int i = 0; i < 16; ++i)
		{
			glm::vec3 noise(
				rand_floats(generator) * 2.0f - 1.0f,
				rand_floats(generator) * 2.0f - 1.0f,
				0.0f
			);
			ssao_noise.push_back(noise);
		}

		glGenTextures(1, &noise_texture);
		glBindTexture(GL_TEXTURE_2D, noise_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssao_noise[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// ssao buffer
		glGenFramebuffers(1, &ssao_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, ssao_buffer);

		glGenTextures(1, &ssao_color);
		glBindTexture(GL_TEXTURE_2D, ssao_color);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width, screen_height, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_color, 0);

		ssao_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/SSAOFragment.shader");
		ssao_shader->use();
		ssao_shader->setInt("gPosition", G_POSITION_TEXTURE);
		ssao_shader->setInt("gNormal", G_NORMAL_TEXTURE);
		ssao_shader->setInt("ssaoNoise", SSAO_NOISE_TEXTURE);

		for (unsigned int i = 0; i < 16; ++i)
		{
			ssao_shader->setVec3("ssao_samples[" + std::to_string(i) + ']', ssao_kernel[i]);
		}

		// ssao blur
		glGenFramebuffers(1, &ssao_blur_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, ssao_blur_buffer);

		glGenTextures(1, &ssao_blur);
		glBindTexture(GL_TEXTURE_2D, ssao_blur);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen_width, screen_height, 0, GL_RED, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssao_blur, 0);

		ssao_blur_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/SSAOBlurFragment.shader");
		ssao_blur_shader->use();
		ssao_blur_shader->setInt("ssaoInput", SSAO_BLUR_INPUT_TEXTURE);
	}

	void initHDRShader()
	{
		hdr_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/HDRFragment.shader");
		hdr_shader->use();
		hdr_shader->setInt("hdrColor", HDR_COLOR_TEXTURE);
		hdr_shader->setInt("bloomColor", BLOOM_COLOR_TEXTURE);
	}

	void initBloom(int screen_width, int screen_height)
	{
		glm::ivec2 src_viewport_size(screen_width, screen_height);
		src_viewport_size_float = glm::vec2((float)screen_width, (float)screen_height);
		bloom_downsample_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/BloomDownsamplingFragment.shader");
		bloom_upsample_shader = new Shader("Shaders/LightingVertex.shader", "Shaders/BloomUpsamplingFragment.shader");

		// check for completeness
		bool bloom_buffer_status = bloom_buffer.init(screen_width, screen_height, num_bloom_mips);
		if (!bloom_buffer_status)
		{
			std::cout << "Failed to initialize bloom buffer!" << std::endl;
		}

		bloom_downsample_shader->use();
		bloom_downsample_shader->setInt("srcTexture", 5);

		bloom_upsample_shader->use();
		bloom_upsample_shader->setInt("srcTexture", 5);
	}

	void renderWorld(World* world)
	{
		world->terrain->draw();
	}

	void updateScreenSize(int new_screen_width, int new_screen_height)
	{
		// update screen size variables
		// update all buffers
	}
};