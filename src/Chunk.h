#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

#include "VertexBufferLayout.h"

// Block Type
enum class BlockType {
	Air, Kusa, Daisy, Tulip, Dandelion, Poppy,   // BillBoard
	Water,	                              //fluid
	Grass, Dirt, Stone, Sand, Wood,       // Cube 
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
	void RenderInitialize(std::vector<std::shared_ptr<Shader>> shader);
	std::shared_ptr<Renderer> GetRenderer() { return m_renderer; };
	int GetBlockTypeID(glm::ivec3 index);

private:
	void LoadBlockTextures();

private:
	int m_ChunkSize;
	std::vector<int> data;

	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<float> m_BillBoardVertices;
	std::vector<unsigned int> m_BillBoardIndices;
	std::vector<float> m_WaterVertices;
	std::vector<unsigned int> m_WaterIndices;

	std::vector<BlockTextureCoordinates> m_BlockTypes;
	std::vector<NoiseSettings> m_NoiseSettings;

	bool m_Generated = false;
	bool m_Initialized = false;
	glm::vec3 m_OriginPos;

	std::vector<std::shared_ptr<VertexArray>>  m_va;
	std::vector<std::shared_ptr<IndexBuffer>>  m_ib;
	std::vector<std::shared_ptr<VertexBuffer>> m_vb;
	std::shared_ptr<Renderer> m_renderer;
};