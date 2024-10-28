#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Renderer.h"

#include <vector>
#include <cmath>
#include <cstdlib>
#include <iostream>

// Block Type
enum class BlockType {
	Air,
	Grass,
	Dirt,
	Stone,
	Water,
	Sand,
	Wood,
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

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed);
    double noise(double x, double y);
private:
	double fade(double t);
	double lerp(double a, double b, double t);
	double grad(int hash, double x, double y);

private:
    int permutation[512];
};

class Chunk
{
public:
	Chunk(unsigned int chunkSize);
	~Chunk();

	void Generate(unsigned int seed);
	std::vector<float> GetVertices() { return m_Vertices; }
	std::vector<unsigned int> GetIndices() { return m_Indices; }

private:
	glm::uvec3 GetXYZ(unsigned int index);
    std::vector<double> generatePerlinNoise(int n, unsigned int seed);
	void LoadBlockTextures();

private:
	unsigned int m_ChunkSize;
	std::vector<unsigned int> data;

	std::vector<float> m_Vertices;
	std::vector<unsigned int> m_Indices;
	std::vector<BlockTextureCoordinates> m_BlockTypes;
};