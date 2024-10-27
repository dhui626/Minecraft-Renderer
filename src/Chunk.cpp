#include "Chunk.h"
#include <glm/gtc/matrix_transform.hpp>

Chunk::Chunk(unsigned int chunkSize, Renderer* renderer)
	:m_Renderer(renderer)
{
	m_ChunkSize = chunkSize;
	data.resize(chunkSize * chunkSize * chunkSize);
	
}

Chunk::~Chunk()
{
	data.clear();
}

glm::uvec3 Chunk::GetXYZ(unsigned int index)
{
	/*      y
            |
            O -- x
           /
          z
	*/
	if (m_ChunkSize == 0)
	{
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
	unsigned int z = index / (m_ChunkSize * m_ChunkSize);
	index -= z * (m_ChunkSize * m_ChunkSize);
	unsigned int y = index / m_ChunkSize;
	index -= y * m_ChunkSize;
	unsigned int x = index;
	return glm::vec3(x, y, z);
}

std::vector<double> Chunk::generatePerlinNoise(int n, unsigned int seed)
{
	PerlinNoise perlin(seed);
	std::vector<double> noiseMap(n * n);

	double frequency = 0.8; // 初始频率
	double amplitude = 0.5; // 初始振幅
	double persistence = 0.3; // 振幅衰减
	double maxValue = 0.0; // 用于归一化最大的值

	// 叠加多个 Perlin 噪声
	for (int octave = 0; octave < 6; ++octave) { // 6 个不同的频率
		for (int y = 0; y < n; ++y) {
			for (int x = 0; x < n; ++x) {
				noiseMap[y * n + x] += perlin.noise(x * frequency / n, y * frequency / n) * amplitude;
			}
		}
		maxValue += amplitude; // 记录最大值
		amplitude *= persistence; // 振幅衰减
		frequency /= 2.0; // 频率增加
	}

	// 归一化噪声图
	for (double& value : noiseMap) {
		value = value / maxValue; // 将值映射到 [0, 1]
	}

	return noiseMap;
}

void Chunk::Generate(unsigned int seed)
{
	// NOTE: y value is the UP axis
	std::vector<double> noiseMap = generatePerlinNoise(m_ChunkSize, seed);

	unsigned int height = 3;
	for (unsigned int z = 0; z < m_ChunkSize; z++)
	{
		for (unsigned int x = 0; x < m_ChunkSize; x++)
		{
			height = (int)(noiseMap[x + z * m_ChunkSize] * m_ChunkSize);
			for (unsigned int y = 0; y < height; y++)
			{
				data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = 1;
			}
		}
	}

}

void Chunk::Render()
{
	// demo
	for (unsigned int i = 0; i < data.size(); i++)
	{
		glm::uvec3 xyz = GetXYZ(i);
		glm::vec3 translation = xyz;
		if (data[i] != false) // has block
		{
			glm::mat4 model = glm::translate(glm::mat4{ 1.0 }, translation);
			Shader* shader = m_Renderer->GetShader();
			shader->SetUniformMat4f("u_Model", model);
			m_Renderer->Draw();
		}
	}
}

PerlinNoise::PerlinNoise(unsigned int seed)
{
	// 初始化随机数生成器
	srand(seed);
	for (int i = 0; i < 256; ++i) {
		permutation[i] = i;
	}
	// 打乱排列
	for (int i = 0; i < 256; ++i) {
		int j = rand() % 256;
		std::swap(permutation[i], permutation[j]);
	}
	// 复制排列
	for (int i = 0; i < 256; ++i) {
		permutation[256 + i] = permutation[i];
	}
}

double PerlinNoise::noise(double x, double y)
{
	int xi = static_cast<int>(std::floor(x)) & 255;
	int yi = static_cast<int>(std::floor(y)) & 255;

	double xf = x - std::floor(x);
	double yf = y - std::floor(y);

	double u = fade(xf);
	double v = fade(yf);

	int aa = permutation[permutation[xi] + yi];
	int ab = permutation[permutation[xi] + yi + 1];
	int ba = permutation[permutation[xi + 1] + yi];
	int bb = permutation[permutation[xi + 1] + yi + 1];

	double x1 = lerp(grad(aa, xf, yf), grad(ab, xf, yf - 1), v);
	double x2 = lerp(grad(ba, xf - 1, yf), grad(bb, xf - 1, yf - 1), v);

	return (lerp(x1, x2, u) + 1) / 2; // 将范围映射到 [0, 1]
}

double PerlinNoise::fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double a, double b, double t)
{
	return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y)
{
	switch (hash & 3) {
	case 0: return  x + y; // Gradient 1
	case 1: return -x + y; // Gradient 2
	case 2: return  x - y; // Gradient 3
	case 3: return -x - y; // Gradient 4
	default: return 0; // Should never happen
	}
}
