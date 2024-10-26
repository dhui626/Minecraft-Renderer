#pragma once

#include <GL/glew.h>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__));

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);


class Renderer
{
public:
	Renderer(VertexArray* va, IndexBuffer* ib, Shader* shader);

	void Clear() const;
	void Draw() const;
	
	VertexArray* GetVAO() const;
	IndexBuffer* GetIBO() const;
	Shader* GetShader() const;

private:
	VertexArray* m_va;
	IndexBuffer* m_ib;
	Shader* m_shader;
};


