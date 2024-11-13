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

// VAO Type
enum class VAOType {
	Solid,
	Billboard,
	Water,
	UNDIFINED
};

class Renderer
{
public:
	Renderer(std::vector<std::shared_ptr<Shader>> shader);

	void Clear() const;
	void Draw() const;
	
	//std::vector<std::shared_ptr<VertexArray>> GetVAO() const { return m_va; };
	//std::vector<std::shared_ptr<IndexBuffer>> GetIBO() const { return m_ib; };
	//std::vector<std::shared_ptr<Shader>> GetShader() const { return m_shader; };

	static unsigned int GetDepthMap() { return m_DepthMap; };
	static unsigned int GetDepthMapFBO() { return m_DepthMapFBO; };

	void SetVAOIBO(std::vector<std::shared_ptr<VertexArray>> va, std::vector<std::shared_ptr<IndexBuffer>> ib);
	void ChangeShader(std::shared_ptr<Shader> shader);
	void ChangeShader(std::vector<std::shared_ptr<Shader>> shaders);

	void GenerateDepthMap();
	
private:
	std::vector<std::shared_ptr<VertexArray>> m_va;
	std::vector<std::shared_ptr<IndexBuffer>> m_ib;
	std::vector<std::shared_ptr<Shader>> m_shader;

	static unsigned int m_DepthMap;
	static unsigned int m_DepthMapFBO;
};


