#pragma once
#include <vector>
#include "Chunk.h"

class World
{
public:
	World(int chunkSize, int distance = 1);
	~World();

	void SetRenderDistance(int distance);
	std::vector<float> GetVertices();
	std::vector<unsigned int> GetIndices();
	void Generate(unsigned int seed);

private:
	int m_RenderDistance = 1;
	int m_ChunkSize;
	std::vector<Chunk*> m_ChunkData;
};
