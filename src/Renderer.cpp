#include "Renderer.h"
#include <iostream>

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

Renderer::Renderer(VertexArray* va, IndexBuffer* ib, Shader* shader)
    :m_va(va), m_ib(ib), m_shader(shader)
{
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::Draw() const
{
    m_shader->Bind();
    m_va->Bind();
    m_ib->Bind();
    GLCall(glDrawElements(GL_TRIANGLES, m_ib->GetCount(), GL_UNSIGNED_INT, nullptr));
}

VertexArray* Renderer::GetVAO() const
{
    return m_va;
}

IndexBuffer* Renderer::GetIBO() const
{
    return m_ib;
}

Shader* Renderer::GetShader() const
{
    return m_shader;
}
