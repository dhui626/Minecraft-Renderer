#pragma once

#include <GL/glew.h>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

class Renderer
{
public:
	Renderer(VertexArray* va, IndexBuffer* ib, Shader* shader);

	void Clear() const;
	void Draw() const;
	
	VertexArray* GetVAO() const { return m_va; };
	IndexBuffer* GetIBO() const { return m_ib; };
	Shader* GetShader() const { return m_shader; };
	unsigned int GetDepthMap() const { return m_DepthMap; };
	unsigned int GetDepthMapFBO() const { return m_DepthMapFBO; };
	void ChangeShader(Shader* shader);

	void GenerateDepthMap();

private:
	VertexArray* m_va;
	IndexBuffer* m_ib;
	Shader* m_shader;

	unsigned int m_DepthMap;
	unsigned int m_DepthMapFBO;
};


