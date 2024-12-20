#pragma once

#include "Renderer.h"

class Texture
{
public:
	Texture(const std::string& path);
	~Texture();

	void AddTexture(const std::string& path, unsigned int slot);
	void BuildTexture(unsigned int slot);

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }

private:
	unsigned int m_RendererID;
	std::vector<std::string> m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
};
