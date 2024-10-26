#include "Chunk.h"
#include <glm/gtc/matrix_transform.hpp>

Chunk::Chunk(unsigned int chunkSize, Renderer* renderer)
	:m_Renderer(renderer)
{
	m_ChunkSize = chunkSize;
	data.resize(chunkSize * chunkSize * chunkSize);
	
}

Chunk::~Chunk()
{
	data.clear();
}

glm::uvec3 Chunk::GetXYZ(unsigned int index)
{
	if (m_ChunkSize == 0)
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
	unsigned int z = index / (m_ChunkSize * m_ChunkSize);
	index -= z * (m_ChunkSize * m_ChunkSize);
	unsigned int y = index / m_ChunkSize;
	index -= y * m_ChunkSize;
	unsigned int x = index;
	return glm::vec3(x, y, z);
}

void Chunk::Generate()
{
	// demo
	for (int z = 0; z < m_ChunkSize; z++)
	{
		for (int y = 0; y < m_ChunkSize; y++)
		{
			for (int x = 0; x < m_ChunkSize; x++)
			{
				data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = 1;
			}
		}
	}
}

void Chunk::Render()
{
	// demo
	for (unsigned int i = 0; i < data.size(); i++)
	{
		glm::uvec3 xyz = GetXYZ(i);
		glm::vec3 translation = xyz;
		if (data[i] != false) // has block
		{
			glm::mat4 model = glm::translate(glm::mat4{ 1.0 }, translation);
			Shader* shader = m_Renderer->GetShader();
			shader->SetUniformMat4f("u_Model", model);
			m_Renderer->Draw();
		}
	}
}
