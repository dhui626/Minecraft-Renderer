#include "Chunk.h"
#include <glm/gtc/matrix_transform.hpp>

Chunk::Chunk(unsigned int chunkSize)
{
	m_ChunkSize = chunkSize;
	data.resize(chunkSize * chunkSize * chunkSize);
    LoadBlockTextures();
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
	double amplitude = 1.0; // 初始振幅
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

void Chunk::LoadBlockTextures()
{
    constexpr int BlockTypeCount = static_cast<int>(BlockType::UNDIFINED);
    m_BlockTypes.resize(BlockTypeCount);
    for (int i = 0; i < BlockTypeCount; i++)
    {
        switch (i)
        {
        case (int)BlockType::Grass:
            m_BlockTypes[i].left   = glm::vec2(25.0f / 64.0f, 23.0f / 32.0f);
            m_BlockTypes[i].right  = glm::vec2(25.0f / 64.0f, 23.0f / 32.0f);
            m_BlockTypes[i].top    = glm::vec2(11.0f / 64.0f, 14.0f / 32.0f);
            m_BlockTypes[i].bottom = glm::vec2(21.0f / 64.0f, 18.0f / 32.0f);
            m_BlockTypes[i].front  = glm::vec2(25.0f / 64.0f, 23.0f / 32.0f);
            m_BlockTypes[i].back   = glm::vec2(25.0f / 64.0f, 23.0f / 32.0f);
            break;
        case (int)BlockType::Dirt:
            m_BlockTypes[i].left   = glm::vec2(21.0f / 64.0f, 18.0f / 32.0f);
            m_BlockTypes[i].right  = glm::vec2(21.0f / 64.0f, 18.0f / 32.0f);
            m_BlockTypes[i].top    = glm::vec2(21.0f / 64.0f, 18.0f / 32.0f);
            m_BlockTypes[i].bottom = glm::vec2(21.0f / 64.0f, 18.0f / 32.0f);
            m_BlockTypes[i].front  = glm::vec2(21.0f / 64.0f, 18.0f / 32.0f);
            m_BlockTypes[i].back   = glm::vec2(21.0f / 64.0f, 18.0f / 32.0f);
            break;
        case (int)BlockType::Stone:
            m_BlockTypes[i].left   = glm::vec2(6.0f / 64.0f, 5.0f / 32.0f);
            m_BlockTypes[i].right  = glm::vec2(6.0f / 64.0f, 5.0f / 32.0f);
            m_BlockTypes[i].top    = glm::vec2(6.0f / 64.0f, 5.0f / 32.0f);
            m_BlockTypes[i].bottom = glm::vec2(6.0f / 64.0f, 5.0f / 32.0f);
            m_BlockTypes[i].front  = glm::vec2(6.0f / 64.0f, 5.0f / 32.0f);
            m_BlockTypes[i].back   = glm::vec2(6.0f / 64.0f, 5.0f / 32.0f);
            break;
        default:
            break;
        }
    }
}

void Chunk::Generate(unsigned int seed)
{
	// NOTE: y value is the UP axis
	std::vector<double> noiseMap = generatePerlinNoise(m_ChunkSize, seed);

    // Generating Step
	unsigned int height = 0;
	for (unsigned int z = 0; z < m_ChunkSize; z++)
	{
		for (unsigned int x = 0; x < m_ChunkSize; x++)
		{
			height = (int)(noiseMap[x + z * m_ChunkSize] * m_ChunkSize);
            
            // block data
            for (unsigned int y = 0; y < height / 2; y++)
                data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Stone;
			for (unsigned int y = height / 2; y < height - 1; y++)
				data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Dirt;
            for (unsigned int y = height - 1; y < height; y++)
                data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Grass;
		}
	}

	m_Vertices.clear();
	m_Indices.clear();

	// Rendering Optimize : BATCH RENDERING
	for (unsigned int z = 0; z < m_ChunkSize; z++)
	{
		for (unsigned int x = 0; x < m_ChunkSize; x++)
		{
			height = (int)(noiseMap[x + z * m_ChunkSize] * m_ChunkSize);
			for (unsigned int y = 0; y < height; y++)
			{
				// 当前方块的位置
				glm::vec3 position(x, y, z);
                unsigned int blockTypeID = data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize];

                // 检查六个面
                // 底面
                if (y == 0 || data[x + (y - 1) * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] == 0) {
                    // 添加底面四个顶点
                    float textureCoordX = m_BlockTypes[blockTypeID].bottom.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].bottom.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x, position.y, position.z,  // 底左下
                        0.0f, -1.0f, 0.0f,                     // 法向量
                        textureCoordX, textureCoordY,         // 纹理坐标

                        position.x + 1.0f, position.y, position.z,  // 底右下
                        0.0f, -1.0f, 0.0f,                           // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY, // 纹理坐标

                        position.x + 1.0f, position.y, position.z + 1.0f,  // 底右上
                        0.0f, -1.0f, 0.0f,                               // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f, // 纹理坐标

                        position.x, position.y, position.z + 1.0f,  // 底左上
                        0.0f, -1.0f, 0.0f,                           // 法向量
                        textureCoordX, textureCoordY + 1.0f / 32.0f  // 纹理坐标
                        });
                }

                // 顶面
                if (y == m_ChunkSize - 1 || data[x + (y + 1) * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] == 0) {
                    // 添加顶面四个顶点
                    float textureCoordX = m_BlockTypes[blockTypeID].top.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].top.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x, position.y + 1.0f, position.z,  // 顶左下
                        0.0f, 1.0f, 0.0f,                       // 法向量
                        textureCoordX, textureCoordY,          // 纹理坐标

                        position.x + 1.0f, position.y + 1.0f, position.z,  // 顶右下
                        0.0f, 1.0f, 0.0f,                               // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY, // 纹理坐标

                        position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,  // 顶右上
                        0.0f, 1.0f, 0.0f,                                       // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f, // 纹理坐标

                        position.x, position.y + 1.0f, position.z + 1.0f,  // 顶左上
                        0.0f, 1.0f, 0.0f,                                   // 法向量
                        textureCoordX, textureCoordY + 1.0f / 32.0f // 纹理坐标
                        });
                }

                // 检查相邻方块以决定侧面
                if (x == 0 || data[(x - 1) + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] == 0) {
                    // 添加左面四个顶点
                    float textureCoordX = m_BlockTypes[blockTypeID].left.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].left.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x, position.y, position.z,  // 左面左下
                        -1.0f, 0.0f, 0.0f,                     // 法向量
                        textureCoordX, textureCoordY,

                        position.x, position.y, position.z + 1.0f,  // 左面左上
                        -1.0f, 0.0f, 0.0f,                         // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x, position.y + 1.0f, position.z + 1.0f,  // 左面右上
                        -1.0f, 0.0f, 0.0f,                               // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                        position.x, position.y + 1.0f, position.z,  // 左面右下
                        - 1.0f, 0.0f, 0.0f,                           // 法向量
                        textureCoordX, textureCoordY + 1.0f / 32.0f
                        });
                }

                if (x == m_ChunkSize - 1 || data[(x + 1) + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] == 0) {
                    // 添加右面四个顶点
                    float textureCoordX = m_BlockTypes[blockTypeID].right.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].right.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x + 1.0f, position.y, position.z,  // 右面左下
                        1.0f, 0.0f, 0.0f,                             // 法向量
                        textureCoordX, textureCoordY,

                        position.x + 1.0f, position.y, position.z + 1.0f,  // 右面左上
                        1.0f, 0.0f, 0.0f,                                   // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,  // 右面右上
                        1.0f, 0.0f, 0.0f,                                         // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                        position.x + 1.0f, position.y + 1.0f, position.z,  // 右面右下
                        1.0f, 0.0f, 0.0f,                                   // 法向量
                        textureCoordX, textureCoordY + 1.0f / 32.0f
                        });
                }

                if (z == 0 || data[x + y * m_ChunkSize + (z - 1) * m_ChunkSize * m_ChunkSize] == 0) {
                    // 添加前面四个顶点
                    float textureCoordX = m_BlockTypes[blockTypeID].front.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].front.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x, position.y, position.z,  // 前面左下
                        0.0f, 0.0f, -1.0f,                     // 法向量
                        textureCoordX, textureCoordY,

                        position.x + 1.0f, position.y, position.z,  // 前面右下
                        0.0f, 0.0f, -1.0f,                         // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x + 1.0f, position.y + 1.0f, position.z,  // 前面右上
                        0.0f, 0.0f, -1.0f,                                 // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                        position.x, position.y + 1.0f, position.z,  // 前面左上
                        0.0f, 0.0f, -1.0f,                           // 法向量
                        textureCoordX, textureCoordY + 1.0f / 32.0f
                        });
                }

                if (z == m_ChunkSize - 1 || data[x + y * m_ChunkSize + (z + 1) * m_ChunkSize * m_ChunkSize] == 0) {
                    // 添加后面四个顶点
                    float textureCoordX = m_BlockTypes[blockTypeID].back.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].back.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x, position.y, position.z + 1.0f,  // 后面左下
                        0.0f, 0.0f, 1.0f,                             // 法向量
                        textureCoordX, textureCoordY,

                        position.x + 1.0f, position.y, position.z + 1.0f,  // 后面右下
                        0.0f, 0.0f, 1.0f,                                   // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,  // 后面右上
                        0.0f, 0.0f, 1.0f,                                       // 法向量
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                        position.x, position.y + 1.0f, position.z + 1.0f,  // 后面左上
                        0.0f, 0.0f, 1.0f,                                   // 法向量
                        textureCoordX, textureCoordY + 1.0f / 32.0f
                        });
                }
			}
		}
	}
    // 生成索引（假设每个面有两个三角形）
    unsigned int vertexCount = m_Vertices.size() / 8; // 每个顶点有 8 个浮点数
    for (unsigned int i = 0; i < vertexCount; i += 4) {
        // 假设每个面有四个顶点
        m_Indices.push_back(i);
        m_Indices.push_back(i + 1);
        m_Indices.push_back(i + 2);
        m_Indices.push_back(i);
        m_Indices.push_back(i + 2);
        m_Indices.push_back(i + 3);
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
