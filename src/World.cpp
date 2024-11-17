#include "World.h"

World::World(int chunkSize, int distance, unsigned int seed)
{
	int gridNum = 2 * distance - 1;

	for (int i = 0; i < gridNum; i++)
	{
		for (int j = 0; j < gridNum; j++)
		{
			std::pair<int, int> key = { i - distance + 1,j - distance + 1 };
			if (m_ChunkData.find(key) == m_ChunkData.end()) // not generated
			{
				m_ChunkQueue.push(key);
			}
		}
	}
	m_ChunkSize = chunkSize;
	m_RenderDistance = distance;
	m_Seed = seed;
	lastChunkPos = glm::vec3(0.0f, 0.0f, 0.0f);
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
	for (auto entry : m_ChunkData)
	{
		auto chunkPtr = entry.second;
		chunkPtr->Generate(seed);
	}
}

void World::Update(std::vector<std::shared_ptr<Shader>> shader, glm::vec3 cameraPos)
{
	if (!m_ChunkQueue.empty())
	{
		std::pair<int, int> key = m_ChunkQueue.front();
		m_ChunkQueue.pop();

		if (m_ChunkData.find(key) == m_ChunkData.end()) // not generated
		{
			auto chunkPtr = std::make_shared<Chunk>(m_ChunkSize,
				glm::vec3(key.first * m_ChunkSize, 0.0f, key.second * m_ChunkSize));
			chunkPtr->Generate(m_Seed);
			chunkPtr->RenderInitialize(shader);
			m_ChunkData[key] = chunkPtr;
		}
	}
	
	int currentChunkX = cameraPos.x < 0 ? cameraPos.x / m_ChunkSize - 1 : cameraPos.x / m_ChunkSize;
	int currentChunkY = cameraPos.y < 0 ? cameraPos.y / m_ChunkSize - 1 : cameraPos.y / m_ChunkSize;
	int currentChunkZ = cameraPos.z < 0 ? cameraPos.z / m_ChunkSize - 1 : cameraPos.z / m_ChunkSize;
	glm::ivec3 currentChunkPos{ currentChunkX ,currentChunkY,currentChunkZ };
	if (currentChunkPos != lastChunkPos)
	{
		// Generate new chunks
		int gridNum = 2 * m_RenderDistance - 1;
		for (int i = 0; i < gridNum; i++)
		{
			for (int j = 0; j < gridNum; j++)
			{
				std::pair<int, int> key = { i - m_RenderDistance + currentChunkPos.x + 1,
					j - m_RenderDistance + currentChunkPos.z + 1 };
				if (m_ChunkData.find(key) == m_ChunkData.end()) // not generated
				{
					m_ChunkQueue.push(key);
				}
			}
		}
		// Delete faraway chunks
		for (auto it = m_ChunkData.begin(); it != m_ChunkData.end();) {
			std::pair<int, int> key = it->first;
			if (abs(key.first - (int)currentChunkPos.x) >= m_RenderDistance ||
				abs(key.second - (int)currentChunkPos.z) >= m_RenderDistance)
			{
				//entry.second->~Chunk();  // Deleted automatically
				it = m_ChunkData.erase(it);
				//std::cout << "x " << (abs(key.first - (int)currentChunkPos.x) >= m_RenderDistance) << std::endl;
				//std::cout << "z " << (abs(key.second - (int)currentChunkPos.z) >= m_RenderDistance) << std::endl;
			}
			else {
				++it;
			}
		}
	}

	lastChunkPos = currentChunkPos;
}

glm::ivec3 World::GetCurrentChunkPos()
{
	return lastChunkPos;
}

size_t World::GetChunkNum()
{
	return m_ChunkData.size();
}

std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, pair_hash> World::GetChunkData()
{
	return m_ChunkData;
}
