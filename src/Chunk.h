#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "Renderer.h"

// Block Type
enum class BlockType {
	Air, Kusa, Daisy, Tulip, Dandelion, Poppy,   // non-cube
	Grass, Dirt, Stone, Water, Sand, Wood,       // Cube 
	UNDIFINED
};

// Texture Coords of each side
struct BlockTextureCoordinates {
	glm::vec2 left;
	glm::vec2 right;
	glm::vec2 top;
	glm::vec2 bottom;
	glm::vec2 front;
	glm::vec2 back;
};

struct NoiseSettings {
	float amplitude;
	float frequency;
	float offset;
};

class Chunk
{
public:
	Chunk(int chunkSize, glm::vec3 originPos);
	~Chunk();

	void Generate(unsigned int seed);
	std::vector<float> GetVertices() { return m_Vertices; }
	std::vector<unsigned int> GetIndices() { return m_Indices; }

private:
	glm::uvec3 GetXYZ(unsigned int index);
	void LoadBlockTextures();

private:
	int m_ChunkSize;
	std::vector<int> data;

	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<BlockTextureCoordinates> m_BlockTypes;
	std::vector<NoiseSettings> m_NoiseSettings;

	bool m_Generated = false;
	glm::vec3 m_OriginPos;
};