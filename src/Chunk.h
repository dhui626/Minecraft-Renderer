#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Renderer.h"

class Chunk
{
public:
	Chunk(unsigned int chunkSize, Renderer* renderer);
	~Chunk();

	glm::uvec3 GetXYZ(unsigned int index);
	void Generate();
	void Render();

private:
	unsigned int m_ChunkSize;
	std::vector<bool> data;
	Renderer* m_Renderer;
};