#pragma once
#include <memory>

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	void Bind() const;
	void Unbind() const;
	void Render() const;

	unsigned int GetID() { return m_RendererID; }
	unsigned int GetFBOTexture() { return m_FrameBufferTexture; }
	unsigned int GetDepthTexture() { return m_DepthTexture; }

private:
	unsigned int m_RendererID;
	unsigned int m_FrameBufferTexture, m_DepthTexture;
	unsigned int m_rectVAO, m_rectVBO;
};