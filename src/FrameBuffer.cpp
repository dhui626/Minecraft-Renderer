#include "FrameBuffer.h"
#include "Renderer.h"

static float rectangleVertices[] =
{
    // Coords     // TexCoords
    1.0f, -1.0f,  1.0f, 0.0f,
   -1.0f, -1.0f,  0.0f, 0.0f,
   -1.0f,  1.0f,  0.0f, 1.0f,

    1.0f,  1.0f,  1.0f, 1.0f,
    1.0f, -1.0f,  1.0f, 0.0f,
   -1.0f,  1.0f,  0.0f, 1.0f
};

FrameBuffer::FrameBuffer(int width, int height)
{
    glGenFramebuffers(1, &m_RendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

    glGenTextures(2, m_FrameBufferTexture);
    for (unsigned int i = 0; i < 2; i++)
    {
        glBindTexture(GL_TEXTURE_2D, m_FrameBufferTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_FrameBufferTexture[i], 0);
    }

    glGenTextures(1, &m_DepthTexture);
    glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer error: " << fboStatus << '\n';

    // Multiple Render Targets
    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glGenVertexArrays(1, &m_rectVAO);
    glGenBuffers(1, &m_rectVBO);
    glBindVertexArray(m_rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    // FBO used for Gaussian Blur
    glGenFramebuffers(2, m_GaussianBlurFBO);
    glGenTextures(2, m_GaussianBlurBuffer);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_GaussianBlurFBO[i]);
        glBindTexture(GL_TEXTURE_2D, m_GaussianBlurBuffer[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_GaussianBlurBuffer[i], 0);
    }
}

FrameBuffer::~FrameBuffer()
{
    glDeleteTextures(1, &m_FrameBufferTexture[0]);
    glDeleteTextures(1, &m_FrameBufferTexture[1]);
    glDeleteTextures(1, &m_DepthTexture);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &m_RendererID);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_rectVAO);
    glDeleteBuffers(1, &m_rectVBO);
}

void FrameBuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
}

void FrameBuffer::Unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Render() const
{
    Unbind();
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(m_rectVAO);
    glActiveTexture(GL_TEXTURE0 + m_FrameBufferTexture[0]);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferTexture[0]);
    glActiveTexture(GL_TEXTURE0 + m_FrameBufferTexture[1]);
    glBindTexture(GL_TEXTURE_2D, m_FrameBufferTexture[1]);
    glActiveTexture(GL_TEXTURE0 + m_DepthTexture);
    glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::GaussianBlur(int amount, std::shared_ptr<Shader> blurShader) const
{
    bool horizontal = true, first_iteration = true;
    blurShader->Bind();
    for (unsigned int i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_GaussianBlurFBO[horizontal]);
        blurShader->SetUniform1i("horizontal", horizontal);
        blurShader->SetUniform1i("image", first_iteration ? m_FrameBufferTexture[1] : m_GaussianBlurBuffer[!horizontal]);
        glActiveTexture(GL_TEXTURE0 + m_GaussianBlurBuffer[!horizontal]);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? m_FrameBufferTexture[1] : m_GaussianBlurBuffer[!horizontal]);
        glBindVertexArray(m_rectVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    Unbind();
}
