#include "Chunk.h"
#include <glm/gtc/matrix_transform.hpp>
#include "vendor/OpenSimplexNoise.hh"

Chunk::Chunk(int chunkSize, glm::vec3 originPos)
{
	m_ChunkSize = chunkSize + 2; // 多计算一圈（用于优化侧表面生成）
	data.resize(m_ChunkSize * m_ChunkSize * m_ChunkSize);
    LoadBlockTextures();
    m_OriginPos = originPos;

    m_NoiseSettings.resize(2);
    m_NoiseSettings[0] = { 20.0f,0.01f,0.0f };
    m_NoiseSettings[1] = { 3.0f,0.05f,0.0f };
}

Chunk::~Chunk()
{
    std::cout << "Deleted chunk at pos(" << m_OriginPos.x << ", " << m_OriginPos.y << ", " << m_OriginPos.z << ")" << std::endl;
}
// 输入实际的index(0-based)，获取data中该位置的方块类型
int Chunk::GetBlockTypeID(glm::ivec3 index)
{
	/*      y
            |
            O -- x
           /
          z
	*/
    int type = data[index.x + 1 + (index.y + 1) * m_ChunkSize + (index.z + 1) * m_ChunkSize * m_ChunkSize];
	return type;
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
        case (int)BlockType::Water:
            m_BlockTypes[i].left   = glm::vec2(63.0f / 64.0f, 0.0f / 32.0f);
            m_BlockTypes[i].right  = glm::vec2(63.0f / 64.0f, 0.0f / 32.0f);
            m_BlockTypes[i].top    = glm::vec2(6.0f  / 64.0f, 28.0f / 32.0f);
            m_BlockTypes[i].bottom = glm::vec2(63.0f / 64.0f, 0.0f / 32.0f);
            m_BlockTypes[i].front  = glm::vec2(63.0f / 64.0f, 0.0f / 32.0f);
            m_BlockTypes[i].back   = glm::vec2(63.0f / 64.0f, 0.0f / 32.0f);
            break;
        default:
            break;
        }
    }
}

void Chunk::Generate(unsigned int seed)
{
	// NOTE: y value is the UP axis

    static OSN::Noise<2> noise2D(seed);
    static int waterLevel = 18;
    //OSN::Noise<3> noise3D(seed);

    // Generating Step
	int height = 0;
	for (int z = 0; z < m_ChunkSize; z++)
	{
		for (int x = 0; x < m_ChunkSize; x++)
		{
            float noiseValue2D = 0.0f;
            float normalizeNum = 0.0f;
            for (int i = 0; i < m_NoiseSettings.size(); i++)
            {
                noiseValue2D += noise2D.eval(
                    (float)((x - 1 + m_OriginPos.x) * m_NoiseSettings[i].frequency) + m_NoiseSettings[i].offset,
                    (float)((z - 1 + m_OriginPos.z) * m_NoiseSettings[i].frequency) + m_NoiseSettings[i].offset)
                    * m_NoiseSettings[i].amplitude;
                normalizeNum += m_NoiseSettings[i].amplitude;
            }
            noiseValue2D = (noiseValue2D + normalizeNum) / 2 / normalizeNum;
            height = (int)(pow(noiseValue2D, 1) * m_ChunkSize);
            
            // block data
            for (int y = 0; y < height / 2; y++)
                data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Stone;
			for (int y = height / 2; y < height - 1; y++)
				data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Dirt;
            // The top is Grass Block
            if (height > 0)
                data[x + (height - 1) * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Grass;
            // Water
            if (height < waterLevel)
            {
                for (int y = height; y < waterLevel; y++)
                    data[x + y * m_ChunkSize + z * m_ChunkSize * m_ChunkSize] = (int)BlockType::Water;
                continue; // no need to generate flowers etc.
            }
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
	for (int z = 0; z < m_ChunkSize - 2; z++)
	{
		for (int x = 0; x < m_ChunkSize - 2; x++)
		{
			for (int y = 0; y < m_ChunkSize - 2; y++)
			{
                int blockTypeID = GetBlockTypeID(glm::ivec3(x, y, z));
                if (blockTypeID == (int)BlockType::Air)
                    continue;

				// 当前方块的位置
				glm::vec3 position(x, y, z);
                position += m_OriginPos; //offset

                if (blockTypeID >= (int)BlockType::Grass) //Block
                {
                    // 检查六个面
                    // 底面（暂时不需要）
                    /*if (y == 0 || GetBlockTypeID(glm::ivec3(x, y - 1, z)) < (int)BlockType::Grass) {
                        // 添加底面四个顶点
                        float textureCoordX = m_BlockTypes[blockTypeID].bottom.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].bottom.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x + 1.0f, position.y, position.z,  // 底右下
                            0.0f, -1.0f, 0.0f,                           // 法向量
                            textureCoordX + 1.0f / 64.0f, textureCoordY, // 纹理坐标

                            position.x, position.y, position.z,  // 底左下
                            0.0f, -1.0f, 0.0f,                     // 法向量
                            textureCoordX, textureCoordY,         // 纹理坐标

                            position.x, position.y, position.z + 1.0f,  // 底左上
                            0.0f, -1.0f, 0.0f,                           // 法向量
                            textureCoordX, textureCoordY + 1.0f / 32.0f,  // 纹理坐标

                            position.x + 1.0f, position.y, position.z + 1.0f,  // 底右上
                            0.0f, -1.0f, 0.0f,                               // 法向量
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f // 纹理坐标
                            });
                    }*/

                    // 顶面
                    if (GetBlockTypeID(glm::ivec3(x, y + 1, z)) < (int)BlockType::Grass) {
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
                    if (GetBlockTypeID(glm::ivec3(x - 1, y, z)) < (int)BlockType::Grass) {
                        // 添加左面四个顶点
                        float textureCoordX = m_BlockTypes[blockTypeID].left.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].left.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x, position.y, position.z + 1.0f,  // 左面左上
                            -1.0f, 0.0f, 0.0f,                         // 法向量
                            textureCoordX + 1.0f / 64.0f, textureCoordY,

                            position.x, position.y, position.z,  // 左面左下
                            -1.0f, 0.0f, 0.0f,                     // 法向量
                            textureCoordX, textureCoordY,

                            position.x, position.y + 1.0f, position.z,  // 左面右下
                            - 1.0f, 0.0f, 0.0f,                           // 法向量
                            textureCoordX, textureCoordY + 1.0f / 32.0f,

                            position.x, position.y + 1.0f, position.z + 1.0f,  // 左面右上
                            -1.0f, 0.0f, 0.0f,                               // 法向量
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f
                            });
                    }

                    if (GetBlockTypeID(glm::ivec3(x + 1, y, z)) < (int)BlockType::Grass) {
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

                    if (GetBlockTypeID(glm::ivec3(x, y, z - 1)) < (int)BlockType::Grass) {
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

                    if (GetBlockTypeID(glm::ivec3(x, y, z + 1)) < (int)BlockType::Grass) {
                        // 添加后面四个顶点
                        float textureCoordX = m_BlockTypes[blockTypeID].back.x;
                        float textureCoordY = m_BlockTypes[blockTypeID].back.y;
                        m_Vertices.insert(m_Vertices.end(), {
                            position.x + 1.0f, position.y, position.z + 1.0f,  // 后面右下
                            0.0f, 0.0f, 1.0f,                                   // 法向量
                            textureCoordX + 1.0f / 64.0f, textureCoordY,

                            position.x, position.y, position.z + 1.0f,  // 后面左下
                            0.0f, 0.0f, 1.0f,                             // 法向量
                            textureCoordX, textureCoordY,

                            position.x, position.y + 1.0f, position.z + 1.0f,  // 后面左上
                            0.0f, 0.0f, 1.0f,                                   // 法向量
                            textureCoordX, textureCoordY + 1.0f / 32.0f,

                            position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,  // 后面右上
                            0.0f, 0.0f, 1.0f,                                       // 法向量
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f
                            });
                    }

                }
                else if (blockTypeID == (int)BlockType::Water) { //water block
                    float textureCoordX = m_BlockTypes[blockTypeID].top.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].top.y;
                    if (GetBlockTypeID(glm::ivec3(x, y + 1, z)) != (int)BlockType::Water)
                    { 
                        // top face
                        m_WaterVertices.insert(m_WaterVertices.end(), {
                            position.x, position.y + 1.0f, position.z,
                            0.0f, 1.0f, 0.0f,
                            textureCoordX, textureCoordY,

                            position.x + 1.0f, position.y + 1.0f, position.z,
                            0.0f, 1.0f, 0.0f,
                            textureCoordX + 1.0f / 64.0f, textureCoordY,

                            position.x + 1.0f, position.y + 1.0f, position.z + 1.0f,
                            0.0f, 1.0f, 0.0f,
                            textureCoordX + 1.0f / 64.0f, textureCoordY + 1.0f / 32.0f,

                            position.x, position.y + 1.0f, position.z + 1.0f,
                            0.0f, 1.0f, 0.0f,
                            textureCoordX, textureCoordY + 1.0f / 32.0f
                            });
                    }
                }
                else {  //non-block
                    float textureCoordX = m_BlockTypes[blockTypeID].front.x;
                    float textureCoordY = m_BlockTypes[blockTypeID].front.y;
                    m_BillBoardVertices.insert(m_BillBoardVertices.end(), {
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
                    
                    textureCoordX = m_BlockTypes[blockTypeID].back.x;
                    textureCoordY = m_BlockTypes[blockTypeID].back.y;
                    m_BillBoardVertices.insert(m_BillBoardVertices.end(), {
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
    vertexCount = m_BillBoardVertices.size() / 8;
    for (unsigned int i = 0; i < vertexCount; i += 4) {
        m_BillBoardIndices.push_back(i);
        m_BillBoardIndices.push_back(i + 1);
        m_BillBoardIndices.push_back(i + 2);
        m_BillBoardIndices.push_back(i);
        m_BillBoardIndices.push_back(i + 2);
        m_BillBoardIndices.push_back(i + 3);
    }
    vertexCount = m_WaterVertices.size() / 8;
    for (unsigned int i = 0; i < vertexCount; i += 4) {
        m_WaterIndices.push_back(i);
        m_WaterIndices.push_back(i + 1);
        m_WaterIndices.push_back(i + 2);
        m_WaterIndices.push_back(i);
        m_WaterIndices.push_back(i + 2);
        m_WaterIndices.push_back(i + 3);
    }

    m_Generated = true;

    std::cout << "Generated chunk at pos(" << m_OriginPos.x << ", " << m_OriginPos.y << ", " << m_OriginPos.z << ")" << std::endl;
}

void Chunk::RenderInitialize(std::vector<std::shared_ptr<Shader>> shader)
{
    // Initialize For Rendering
    { // Solid
        // VAO
        auto vao = std::make_shared<VertexArray>();
        vao->Bind();
        // VBO
        auto vbo = std::make_shared<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(float));
        // IBO
        auto ibo = std::make_shared<IndexBuffer>(m_Indices.data(), m_Indices.size());

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        vao->AddBuffer(*vbo, layout);

        m_va.push_back(vao);
        m_vb.push_back(vbo);
        m_ib.push_back(ibo);
    }
    { // BillBoard
        auto vao = std::make_shared<VertexArray>();
        vao->Bind();
        auto vbo = std::make_shared<VertexBuffer>(m_BillBoardVertices.data(), m_BillBoardVertices.size() * sizeof(float));
        auto ibo = std::make_shared<IndexBuffer>(m_BillBoardIndices.data(), m_BillBoardIndices.size());
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        vao->AddBuffer(*vbo, layout);

        m_va.push_back(vao);
        m_vb.push_back(vbo);
        m_ib.push_back(ibo);
    }
    { // Water
        auto vao = std::make_shared<VertexArray>();
        vao->Bind();
        auto vbo = std::make_shared<VertexBuffer>(m_WaterVertices.data(), m_WaterVertices.size() * sizeof(float));
        auto ibo = std::make_shared<IndexBuffer>(m_WaterIndices.data(), m_WaterIndices.size());
        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        vao->AddBuffer(*vbo, layout);

        m_va.push_back(vao);
        m_vb.push_back(vbo);
        m_ib.push_back(ibo);
    }

    //Bind shader file
    m_renderer = std::make_shared<Renderer>(shader);
    m_renderer->SetVAOIBO(m_va, m_ib);
    m_renderer->GenerateDepthMap();

    m_Initialized = true;
}

