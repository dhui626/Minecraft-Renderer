#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Renderer.h"

#include <vector>
#include <cmath>
#include <cstdlib>
#include <iostream>

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
	Chunk(unsigned int chunkSize, Renderer* renderer);
	~Chunk();

	glm::uvec3 GetXYZ(unsigned int index);
    std::vector<double> generatePerlinNoise(int n, unsigned int seed);
	void Generate(unsigned int seed);
	void Render();

private:
	unsigned int m_ChunkSize;
	std::vector<bool> data;
	Renderer* m_Renderer;
};