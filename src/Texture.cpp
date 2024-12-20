#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image/stb_image.h"

#include <iostream>

Texture::Texture(const std::string& path)
	:m_RendererID(0), m_FilePath(0), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	m_FilePath.push_back(path);
	// SLOT 0: no need to call glActiveTexture
	BuildTexture(0);
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

float positions[] = {
	// vertex        //coords
	0.0f, 0.0f, 0.0f,  0.0f, 0.0f, // 0
	1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // 1
	1.0f, 1.0f, 0.0f,  1.0f, 1.0f, // 2
	0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // 3
	0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // 4
	1.0f, 0.0f, 1.0f,  1.0f, 0.0f, // 5
	1.0f, 1.0f, 1.0f,  1.0f, 1.0f, // 6
	0.0f, 1.0f, 1.0f,  0.0f, 1.0f  // 7
};

void Texture::AddTexture(const std::string& path, unsigned int slot)
{
	m_FilePath.push_back(path);
	ASSERT(m_FilePath.size() == slot + 1); // avoid wrong slot
	BuildTexture(slot);
}

void Texture::BuildTexture(unsigned int slot)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(m_FilePath[slot].c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
