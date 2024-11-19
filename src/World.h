#pragma once
#include <vector>
#include <unordered_map>
#include <queue>
#include <utility>
#include <functional>

#include "Chunk.h"

struct pair_hash {
	template <class T1, class T2>
	std::size_t operator () (const std::pair<T1, T2>& pair) const {
		auto hash1 = std::hash<T1>{}(pair.first);
		auto hash2 = std::hash<T2>{}(pair.second);
		return hash1 ^ hash2;
	}
};

class World
{
public:
	World(int chunkSize, int distance = 1, unsigned int seed = 0);
	~World();

	void SetRenderDistance(int distance);
	int GetRenderDistance() { return m_RenderDistance; };
	void Generate(unsigned int seed);
	void Update(std::vector<std::shared_ptr<Shader>> shader, glm::vec3 cameraPos);
	glm::ivec3 GetCurrentChunkPos();
	BlockType GetBlockType(glm::vec3 pos);

	size_t GetChunkNum();
	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, pair_hash> GetChunkData();

private:
	int m_RenderDistance = 1, m_lastRenderDistance = 0;
	int m_ChunkSize;
	unsigned int m_Seed = 0;
	glm::ivec3 lastChunkPos;

	std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, pair_hash> m_ChunkData;
	std::queue<std::pair<int, int>> m_ChunkQueue;
};
