#pragma once
#include <memory>
#include "Shader.h"

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	~FrameBuffer();

	void Bind() const;
	void Unbind() const;
	void Render() const;
	void GaussianBlur(int amount, std::shared_ptr<Shader> blurShader) const;

	unsigned int GetID() { return m_RendererID; }
	unsigned int* GetFBOTexture() { return m_FrameBufferTexture; }
	unsigned int GetBlurTexture() { return m_GaussianBlurBuffer[1]; }
	unsigned int GetDepthTexture() { return m_DepthTexture; }

private:
	unsigned int m_RendererID;
	unsigned int m_FrameBufferTexture[2], m_DepthTexture;
	unsigned int m_GaussianBlurFBO[2], m_GaussianBlurBuffer[2];
	unsigned int m_rectVAO, m_rectVBO;
};