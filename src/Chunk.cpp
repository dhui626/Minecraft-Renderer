#include "Chunk.h"
#include <glm/gtc/matrix_transform.hpp>


Chunk::Chunk(unsigned int chunkSize, glm::vec3 originPos)
{
	m_ChunkSize = chunkSize;
	data.resize(chunkSize * chunkSize * chunkSize);
    LoadBlockTextures();
    m_OriginPos = originPos;
    std::cout << "Loaded chunk at pos(" << m_OriginPos.x << ", " << m_OriginPos.y << ", " << m_OriginPos.z << ")" << std::endl;
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

	double frequency = 1.0; // ��ʼƵ��
	double amplitude = 2.0; // ��ʼ���
	double persistence = 0.3; // ���˥��
	double maxValue = 0.0; // ���ڹ�һ������ֵ

	// ���Ӷ�� Perlin ����
	for (int octave = 0; octave < 6; ++octave) { // 6 ����ͬ��Ƶ��
		for (int y = 0; y < n; ++y) {
			for (int x = 0; x < n; ++x) {
				noiseMap[y * n + x] += perlin.noise(x * frequency / n, y * frequency / n) * amplitude;
			}
		}
		maxValue += amplitude; // ��¼���ֵ
		amplitude *= persistence; // ���˥��
		frequency /= 2.0; // Ƶ������
	}

	// ��һ������ͼ
	for (double& value : noiseMap) {
		value = value / maxValue; // ��ֵӳ�䵽 [0, 1]
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
        case (int)BlockType::Kusa:
            m_BlockTypes[i].front = glm::vec2(10.0f / 64.0f, 7.0f / 32.0f);
            m_BlockTypes[i].back =  glm::vec2(10.0f / 64.0f, 7.0f / 32.0f);
            break;
        case (int)BlockType::Daisy:
            m_BlockTypes[i].front = glm::vec2(29.0f / 64.0f, 13.0f / 32.0f);
            m_BlockTypes[i].back =  glm::vec2(29.0f / 64.0f, 13.0f / 32.0f);
            break;
        case (int)BlockType::Tulip:
            m_BlockTypes[i].front = glm::vec2(17.0f / 64.0f, 12.0f / 32.0f);
            m_BlockTypes[i].back =  glm::vec2(17.0f / 64.0f, 12.0f / 32.0f);
            break;
        case (int)BlockType::Dandelion:
            m_BlockTypes[i].front = glm::vec2(18.0f / 64.0f, 28.0f / 32.0f);
            m_BlockTypes[i].back =  glm::vec2(18.0f / 64.0f, 28.0f / 32.0f);
            break;
        case (int)BlockType::Poppy:
            m_BlockTypes[i].front = glm::vec2(21.0f / 64.0f, 11.0f / 32.0f);
            m_BlockTypes[i].back =  glm::vec2(21.0f / 64.0f, 11.0f / 32.0f);
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
	int height = 0;
	for (int z = 0; z < m_ChunkSize; z++)
	{
		for (int x = 0; x < m_ChunkSize; x++)
		{
            height = (int)(pow(noiseMap[x + z * m_ChunkSize], 2.5) * m_ChunkSize);
            
            // block data
            for (int y = 0; y < height / 2; y++)
                data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Stone;
			for (int y = height / 2; y < height - 1; y++)
				data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Dirt;
            // The top is Grass Block
            if (height > 0)
                data[x + (height - 1) * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Grass;
            // Random Kusa
            if (rand() % 20 == 0 && height != 0 && height <= m_ChunkSize)
                data[x + height * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Kusa;
            // Random Flower
            if (rand() % 50 == 0 && height != 0 && height <= m_ChunkSize)
            {
                int flowerTypeNum = (int)BlockType::Grass - (int)BlockType::Daisy;
                data[x + height * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Kusa + rand() % flowerTypeNum;
            }
		}
	}

	m_Vertices.clear();
	m_Indices.clear();

	// Rendering Optimize : BATCH RENDERING
	for (int z = 0; z < m_ChunkSize; z++)
	{
		for (int x = 0; x < m_ChunkSize; x++)
		{
            height = (int)(pow(noiseMap[x + z * m_ChunkSize], 2.5) * m_ChunkSize);
			for (int y = 0; y < m_ChunkSize; y++)
			{
                int blockTypeID = data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize];
                if (blockTypeID == (int)BlockType::Air)
                    continue;

				// ��ǰ�����λ��
				glm::vec3 position(x, y, z);
                position += m_OriginPos; //offset

                if (blockTypeID >= (int)BlockType::Grass) //Block
                {
                    // ���������
                    // ����
                    if (y == 0 || data[x + (y - 1) * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] < (int)BlockType::Grass) {
                        // ��ӵ����ĸ�����
                        float textureCoordX = m_BlockTypes[blockTypeID].bottom.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].bottom.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x + 1.0f, position.y, position.z,  // ������
                            0.0f, -1.0f, 0.0f,                           // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY, // ��������

                            position.x, position.y, position.z,  // ������
                            0.0f, -1.0f, 0.0f,                     // ������
                            textureCoordX, textureCoordY,         // ��������

                            position.x, position.y, position.z + 1.0f,  // ������
                            0.0f, -1.0f, 0.0f,                           // ������
                            textureCoordX, textureCoordY + 1.0f / 32.0f,  // ��������

                            position.x + 1.0f, position.y, position.z + 1.0f,  // ������
                            0.0f, -1.0f, 0.0f,                               // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f // ��������
                            });
                    }

                    // ����
                    if (y == m_ChunkSize - 1 || data[x + (y + 1) * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] < (int)BlockType::Grass) {
                        // ��Ӷ����ĸ�����
                        float textureCoordX = m_BlockTypes[blockTypeID].top.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].top.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x, position.y + 1.0f, position.z,  // ������
                            0.0f, 1.0f, 0.0f,                       // ������
                            textureCoordX, textureCoordY,          // ��������

                            position.x + 1.0f, position.y + 1.0f, position.z,  // ������
                            0.0f, 1.0f, 0.0f,                               // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY, // ��������

                            position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,  // ������
                            0.0f, 1.0f, 0.0f,                                       // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f, // ��������

                            position.x, position.y + 1.0f, position.z + 1.0f,  // ������
                            0.0f, 1.0f, 0.0f,                                   // ������
                            textureCoordX, textureCoordY + 1.0f / 32.0f // ��������
                            });
                    }

                    // ������ڷ����Ծ�������
                    if (x == 0 || data[(x - 1) + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] < (int)BlockType::Grass) {
                        // ��������ĸ�����
                        float textureCoordX = m_BlockTypes[blockTypeID].left.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].left.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x, position.y, position.z + 1.0f,  // ��������
                            -1.0f, 0.0f, 0.0f,                         // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY,

                            position.x, position.y, position.z,  // ��������
                            -1.0f, 0.0f, 0.0f,                     // ������
                            textureCoordX, textureCoordY,

                            position.x, position.y + 1.0f, position.z,  // ��������
                            - 1.0f, 0.0f, 0.0f,                           // ������
                            textureCoordX, textureCoordY + 1.0f / 32.0f,

                            position.x, position.y + 1.0f, position.z + 1.0f,  // ��������
                            -1.0f, 0.0f, 0.0f,                               // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f
                            });
                    }

                    if (x == m_ChunkSize - 1 || data[(x + 1) + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] < (int)BlockType::Grass) {
                        // ��������ĸ�����
                        float textureCoordX = m_BlockTypes[blockTypeID].right.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].right.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x + 1.0f, position.y, position.z,  // ��������
                            1.0f, 0.0f, 0.0f,                             // ������
                            textureCoordX, textureCoordY,

                            position.x + 1.0f, position.y, position.z + 1.0f,  // ��������
                            1.0f, 0.0f, 0.0f,                                   // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY,

                            position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,  // ��������
                            1.0f, 0.0f, 0.0f,                                         // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                            position.x + 1.0f, position.y + 1.0f, position.z,  // ��������
                            1.0f, 0.0f, 0.0f,                                   // ������
                            textureCoordX, textureCoordY + 1.0f / 32.0f
                            });
                    }

                    if (z == 0 || data[x + y * m_ChunkSize + (z - 1) * m_ChunkSize * m_ChunkSize] < (int)BlockType::Grass) {
                        // ���ǰ���ĸ�����
                        float textureCoordX = m_BlockTypes[blockTypeID].front.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].front.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x, position.y, position.z,  // ǰ������
                            0.0f, 0.0f, -1.0f,                     // ������
                            textureCoordX, textureCoordY,

                            position.x + 1.0f, position.y, position.z,  // ǰ������
                            0.0f, 0.0f, -1.0f,                         // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY,

                            position.x + 1.0f, position.y + 1.0f, position.z,  // ǰ������
                            0.0f, 0.0f, -1.0f,                                 // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                            position.x, position.y + 1.0f, position.z,  // ǰ������
                            0.0f, 0.0f, -1.0f,                           // ������
                            textureCoordX, textureCoordY + 1.0f / 32.0f
                            });
                    }

                    if (z == m_ChunkSize - 1 || data[x + y * m_ChunkSize + (z + 1) * m_ChunkSize * m_ChunkSize] < (int)BlockType::Grass) {
                        // ��Ӻ����ĸ�����
                        float textureCoordX = m_BlockTypes[blockTypeID].back.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].back.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x + 1.0f, position.y, position.z + 1.0f,  // ��������
                            0.0f, 0.0f, 1.0f,                                   // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY,

                            position.x, position.y, position.z + 1.0f,  // ��������
                            0.0f, 0.0f, 1.0f,                             // ������
                            textureCoordX, textureCoordY,

                            position.x, position.y + 1.0f, position.z + 1.0f,  // ��������
                            0.0f, 0.0f, 1.0f,                                   // ������
                            textureCoordX, textureCoordY + 1.0f / 32.0f,

                            position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,  // ��������
                            0.0f, 0.0f, 1.0f,                                       // ������
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f
                            });
                    }

                }
                else {  //non-block
                    float textureCoordX = m_BlockTypes[blockTypeID].front.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].front.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x, position.y, position.z,
                        1.0f, 0.0f, -1.0f,
                        textureCoordX, textureCoordY,

                        position.x + 1.0f, position.y, position.z + 1.0f,
                        1.0f, 0.0f, -1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,
                        1.0f, 0.0f, -1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                        position.x, position.y + 1.0f, position.z,
                        1.0f, 0.0f, -1.0f,
                        textureCoordX, textureCoordY + 1.0f / 32.0f
                        });
                    
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x + 1.0f, position.y, position.z + 1.0f,
                        -1.0f, 0.0f, 1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x, position.y, position.z,
                        -1.0f, 0.0f, 1.0f,
                        textureCoordX, textureCoordY,

                        position.x, position.y + 1.0f, position.z,
                        -1.0f, 0.0f, 1.0f,
                        textureCoordX, textureCoordY + 1.0f / 32.0f,

                        position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,
                        -1.0f, 0.0f, 1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f
                        });

                    textureCoordX = m_BlockTypes[blockTypeID].back.x;
                    textureCoordY = m_BlockTypes[blockTypeID].back.y;
                    m_Vertices.insert(m_Vertices.end(), {
                        position.x, position.y, position.z + 1.0f,
                        -1.0f, 0.0f, -1.0f,
                        textureCoordX, textureCoordY,

                        position.x + 1.0f, position.y, position.z,
                        -1.0f, 0.0f, -1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x + 1.0f, position.y + 1.0f, position.z,
                        -1.0f, 0.0f, -1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                        position.x, position.y + 1.0f, position.z + 1.0f,
                        -1.0f, 0.0f, -1.0f,
                        textureCoordX, textureCoordY + 1.0f / 32.0f
                        });

                    m_Vertices.insert(m_Vertices.end(), {
                        position.x + 1.0f, position.y, position.z,
                        1.0f, 0.0f, 1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY,

                        position.x, position.y, position.z + 1.0f,
                        1.0f, 0.0f, 1.0f,
                        textureCoordX, textureCoordY,

                        position.x, position.y + 1.0f, position.z + 1.0f,
                        1.0f, 0.0f, 1.0f,
                        textureCoordX, textureCoordY + 1.0f / 32.0f,

                        position.x + 1.0f, position.y + 1.0f, position.z,
                        1.0f, 0.0f, 1.0f,
                        textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f
                        });
                }
			}
		}
	}

    // ��������������ÿ���������������Σ�
    unsigned int vertexCount = m_Vertices.size() / 8; // ÿ�������� 8 ��������
    for (unsigned int i = 0; i < vertexCount; i += 4) {
        // ����ÿ�������ĸ�����
        m_Indices.push_back(i);
        m_Indices.push_back(i + 1);
        m_Indices.push_back(i + 2);
        m_Indices.push_back(i);
        m_Indices.push_back(i + 2);
        m_Indices.push_back(i + 3);
    }

    m_Generated = true;
}

PerlinNoise::PerlinNoise(unsigned int seed)
{
	// ��ʼ�������������
	srand(seed);
	for (int i = 0; i < 256; ++i) {
		permutation[i] = i;
	}
	// ��������
	for (int i = 0; i < 256; ++i) {
		int j = rand() % 256;
		std::swap(permutation[i], permutation[j]);
	}
	// ��������
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

	return (lerp(x1, x2, u) + 1) / 2; // ����Χӳ�䵽 [0, 1]
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
