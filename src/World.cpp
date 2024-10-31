#include "World.h"

World::World(int chunkSize, int distance)
{
	int gridNum = 2 * distance - 1;
	m_ChunkData.resize(gridNum * gridNum);
	for (int i = 0; i < gridNum; i++)
	{
		for (int j = 0; j < gridNum; j++)
		{
			m_ChunkData[i * gridNum + j] = std::make_shared<Chunk>(chunkSize,
				glm::vec3((i - distance + 1) * chunkSize, 0.0f, (j - distance + 1) * chunkSize));
		}
	}
	m_ChunkSize = chunkSize;
}

World::~World()
{
	m_ChunkData.clear();
}

void World::SetRenderDistance(int distance)
{
	m_RenderDistance = distance;
}

void World::Generate(unsigned int seed)
{
	for (auto chunkPtr : m_ChunkData)
	{
		chunkPtr->Generate(seed);
	}
}

void World::BindShader(Shader* shader)
{
	for (auto chunkPtr : m_ChunkData)
	{
		chunkPtr->RenderInitialize(shader);
	}
}

size_t World::GetChunkNum()
{
	return m_ChunkData.size();
}

std::vector<std::shared_ptr<Chunk>> World::GetChunkData()
{
	return m_ChunkData;
}
