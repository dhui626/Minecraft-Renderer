#pragma once

#include <GL/glew.h>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include <iostream>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048

class Renderer
{
public:
	Renderer(std::shared_ptr<VertexArray> va, std::shared_ptr<IndexBuffer> ib, Shader* shader);

	void Clear() const;
	void Draw() const;
	
	std::shared_ptr<VertexArray> GetVAO() const { return m_va; };
	std::shared_ptr<IndexBuffer> GetIBO() const { return m_ib; };
	Shader* GetShader() const { return m_shader; };
	unsigned int GetDepthMap() const { return m_DepthMap; };
	unsigned int GetDepthMapFBO() const { return m_DepthMapFBO; };
	void ChangeShader(Shader* shader);

	void GenerateDepthMap();

private:
	std::shared_ptr<VertexArray> m_va;
	std::shared_ptr<IndexBuffer> m_ib;
	Shader* m_shader;

	unsigned int m_DepthMap;
	unsigned int m_DepthMapFBO;
};


