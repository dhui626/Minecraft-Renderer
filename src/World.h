#pragma once
#include <vector>
#include "Chunk.h"

class World
{
public:
	World(int chunkSize, int distance = 1);
	~World();

	void SetRenderDistance(int distance);
	void Generate(unsigned int seed);
	void BindShader(Shader* shader);

	size_t GetChunkNum();
	std::vector< std::shared_ptr<Chunk> > GetChunkData();

private:
	int m_RenderDistance = 1;
	int m_ChunkSize;
	std::vector< std::shared_ptr<Chunk> > m_ChunkData;
};
