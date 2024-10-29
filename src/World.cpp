#include "World.h"

World::World(int chunkSize, int distance)
{
	int gridNum = 2 * distance - 1;
	m_ChunkData.resize(gridNum * gridNum);
	for (int i = 0; i < gridNum; i++)
	{
		for (int j = 0; j < gridNum; j++)
		{
			m_ChunkData[i * gridNum + j] = new Chunk(chunkSize,
				glm::vec3((i - distance + 1) * chunkSize, 0.0f, (j - distance + 1) * chunkSize));
		}
	}
	m_ChunkSize = chunkSize;
}

World::~World()
{
	for (int i = 0; i < m_ChunkData.size(); i++)
	{
		delete m_ChunkData[i];
	}
	m_ChunkData.clear();
}

void World::SetRenderDistance(int distance)
{
	m_RenderDistance = distance;
}

std::vector<float> World::GetVertices()
{
	// memory boom!
	std::vector<float> result;
	//result.reserve(m_ChunkSize * m_ChunkSize * m_ChunkSize * m_RenderDistance * m_RenderDistance);
	for (int i = 0; i < m_ChunkData.size(); i++)
	{
		Chunk* chunkPtr = m_ChunkData[i];
		std::vector<float> toInsert = chunkPtr->GetVertices();
		result.insert(result.end(), toInsert.begin(), toInsert.end());
	}
	return result;
}

std::vector<unsigned int> World::GetIndices()
{
	// memory boom!
	std::vector<unsigned int> result;
	//result.reserve(m_ChunkSize * m_ChunkSize * m_ChunkSize * m_RenderDistance * m_RenderDistance);
	unsigned int offset = 0;
	for (int i = 0; i < m_ChunkData.size(); i++)
	{
		Chunk* chunkPtr = m_ChunkData[i];
		std::vector<unsigned int> toInsert = chunkPtr->GetIndices();
		for (size_t j = 0; j < toInsert.size(); j++) {
			toInsert[j] += offset;
		}
		offset += chunkPtr->GetVertices().size() / 8;
		result.insert(result.end(), toInsert.begin(), toInsert.end());
	}
	return result;
}

void World::Generate(unsigned int seed)
{
	for (Chunk* chunkPtr : m_ChunkData)
	{
		chunkPtr->Generate(seed + (int)chunkPtr);
	}
}
