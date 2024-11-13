#include "Renderer.h"

unsigned int Renderer::m_DepthMap = 0;
unsigned int Renderer::m_DepthMapFBO = 0;

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")"
            << "\nFunction: " << function << "\nFile: " << file << "\nLine: " << line << std::endl;
        return false;
    }
    return true;
}

Renderer::Renderer(std::vector<std::shared_ptr<Shader>> shader)
    :m_shader(shader)
{
    m_va.reserve((int)VAOType::UNDIFINED);
    m_ib.reserve((int)VAOType::UNDIFINED);
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw() const
{
    glEnable(GL_DEPTH_TEST);
    // Back Face Culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    for (int i = 0; i < (int)VAOType::UNDIFINED; i++)
    {
        m_shader[i]->Bind();
        m_va[i]->Bind();
        m_ib[i]->Bind();
        switch (i)
        {
        case (int)VAOType::Solid:
            GLCall(glDrawElements(GL_TRIANGLES, m_ib[i]->GetCount(), GL_UNSIGNED_INT, nullptr));
            break;
        case (int)VAOType::Billboard:
            glDisable(GL_CULL_FACE);
            GLCall(glDrawElements(GL_TRIANGLES, m_ib[i]->GetCount(), GL_UNSIGNED_INT, nullptr));
            break;
        case (int)VAOType::Water:
            glEnable(GL_CULL_FACE);
            GLCall(glDrawElements(GL_TRIANGLES, m_ib[i]->GetCount(), GL_UNSIGNED_INT, nullptr));
            break;
        default:
            break;
        }
    }
}

void Renderer::SetVAOIBO(std::vector<std::shared_ptr<VertexArray>> va, std::vector<std::shared_ptr<IndexBuffer>> ib)
{
    assert(va.size() == (int)VAOType::UNDIFINED);
    m_va.clear();
    m_ib.clear();
    for (int i = 0; i < (int)VAOType::UNDIFINED; i++)
    {
        m_va.push_back(va[i]);
        m_ib.push_back(ib[i]);
    }
}

void Renderer::ChangeShader(std::shared_ptr<Shader> shader)
{
    for (int i = 0; i < m_shader.size(); i++)
    {
        m_shader[i] = shader;
    }
}

void Renderer::ChangeShader(std::vector<std::shared_ptr<Shader>> shaders)
{
    m_shader = shaders;
}

void Renderer::GenerateDepthMap()
{
    if (m_DepthMap != 0 && m_DepthMapFBO != 0) // Generated
        return;

    glGenFramebuffers(1, &m_DepthMapFBO);
    glGenTextures(1, &m_DepthMap);
    glBindTexture(GL_TEXTURE_2D, m_DepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
