#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "FrameBuffer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"
#include "World.h"

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

int width = 1280;
int height = 720;
std::shared_ptr<FrameBuffer> fboPtr;

struct Settings
{
    float cameraSpeed = 2.0f;
    bool Shadow = true;
    bool PCF = true;
    bool PCSS = false;
    bool VSync = false;
    float brightness = 0.0f;
    float contrast = 1.0f;
    float saturation = 1.2f;
    float gamma = 1.8f;
    float exposure = 1.5f;
    bool foggy = true;
    bool bloom = true;

    bool toneMapping = true;
    bool reinhard = false;
    bool ce = false;
    bool filmic = true;
    bool aces = false;

    bool waterGeometry = false;
};

void framebufferSizeCallback(GLFWwindow* window, int newW, int newH)
{
    width = newW;
    height = newH;
    glViewport(0, 0, width, height);

    fboPtr->Resize(width, height);
 }

int main(void)
{
    GLFWwindow* window;
    Settings settings;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */

    window = glfwCreateWindow(width, height, "Mini Minecraft Renderer", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(settings.VSync ? 1 : 0);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    /* create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points*/
    if (glewInit() != GLEW_OK)
        return -1;

    std::cout << "OpenGL Version:\t" << glGetString(GL_VERSION) << std::endl;

    /* ImGui Setup */
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
    {
        // Generate Terrains
        int renderDistance = 2;
        World world(32, renderDistance, 1666154);

        /* Camera */ 
        Camera camera(45.0f, 0.1f, 200.0f, window);
        camera.OnResize(width, height);
        camera.SetPosition(glm::vec3(0.0, 20.0, 0.0));
        //MVP matrix
        glm::mat4 model(1.0f);
        glm::vec3 translation{ 0.0f, 0.0f, 0.0f };
        glm::mat4 view = camera.GetView();
        glm::mat4 proj = camera.GetProjection();

        //Basic shader
        std::vector<std::shared_ptr<Shader>> allShaders;
        std::shared_ptr<Shader> shader = std::make_shared<Shader>("res/shaders/Basic.shader");
        allShaders.emplace_back(shader);
        shader->Bind();
        // Uniforms
        glm::vec3 lightPos{ 100.0f,300.0f,200.0f };
        glm::vec3 lightDir{ -1.0f, -3.0f, -2.0f };
        float lightIntensity = 3.0f;
        shader->SetUniform1f("u_LightIntensity", lightIntensity);
        float Kd = 0.3f; //diffuse K
        shader->SetUniform1f("u_Kd", Kd);
        float Ks = 0.05f; //specular K
        shader->SetUniform1f("u_Ks", Ks);

        // Texture
        Texture texture("res/textures/23w31a_blocks.png-atlas.png");
        texture.Bind();
        shader->SetUniform1i("u_Texture", 0);

        // Shadow Map shader
        std::shared_ptr<Shader> shadowShader = std::make_shared<Shader>("res/shaders/Shadow.shader");
        //Shaders.emplace_back(shadowShader);
        shadowShader->Bind();
        // Uniforms
        float near_plane = 0.1f, far_plane = 200.0f;
        glm::mat4 lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        shadowShader->SetUniformMat4f("u_Model", model);
        shadowShader->SetUniformMat4f("u_LightPV", lightSpaceMatrix);
        shadowShader->Unbind();

        // BillBoard shader
        std::shared_ptr<Shader> billBoardShader = std::make_shared<Shader>("res/shaders/BillBoard.shader");
        allShaders.emplace_back(billBoardShader);
        billBoardShader->Bind();
        billBoardShader->SetUniform1f("u_LightIntensity", lightIntensity);
        billBoardShader->SetUniform1f("u_Kd", Kd);
        billBoardShader->SetUniform1f("u_Ks", Ks);
        billBoardShader->SetUniform1i("u_Texture", 0);

        // Water shader
        std::shared_ptr<Shader> waterShader = std::make_shared<Shader>("res/shaders/Water.shader");
        allShaders.emplace_back(waterShader);
        waterShader->Bind();
        waterShader->SetUniform1f("u_LightIntensity", lightIntensity);
        waterShader->SetUniform1f("u_Kd", Kd);
        waterShader->SetUniform1f("u_Ks", Ks);
        waterShader->SetUniform1i("u_Texture", 0);

        // Post-Processing FBO
        std::shared_ptr<Shader> frameBufferShader = std::make_shared<Shader>("res/shaders/FrameBuffer.shader");
        static FrameBuffer fbo(width, height);
        fboPtr = std::make_shared<FrameBuffer>(fbo);

        frameBufferShader->Bind();
        frameBufferShader->SetUniform1i("screenTexture", fbo.GetFBOTexture()[0]);
        frameBufferShader->SetUniform1i("brightTexture", fbo.GetBlurTexture());
        frameBufferShader->SetUniform1i("depthTexture", fbo.GetDepthTexture());

        // Gaussian Blur shader
        std::shared_ptr<Shader> blurShader = std::make_shared<Shader>("res/shaders/GaussianBlur.shader");

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Press Input */
            float currentTime = static_cast<float>(glfwGetTime());
            deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;
            camera.OnResize(width, height);
            camera.OnUpdate(settings.cameraSpeed * deltaTime);
            glfwSwapInterval(settings.VSync ? 1 : 0);

            /* Render here */
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            if(world.GetRenderDistance() != renderDistance)
                world.SetRenderDistance(renderDistance);
            world.Update(allShaders, camera.GetPosition());
            auto chunkData = world.GetChunkData();

            unsigned int DepthFBO = 0;
            unsigned int DepthMapID = 0;
            glm::uvec3 currentChunkPos = world.GetCurrentChunkPos();
            std::pair<int, int> currentChunkXZ{ currentChunkPos.x, currentChunkPos.z };
            if (chunkData.find(currentChunkXZ) != chunkData.end())
            {
                std::shared_ptr<Renderer> renderer = chunkData.at(currentChunkXZ)->GetRenderer();
                DepthFBO = renderer->GetDepthMapFBO();
                DepthMapID = renderer->GetDepthMap();
            }

            if (settings.Shadow)
            {
                glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
                glBindFramebuffer(GL_FRAMEBUFFER, DepthFBO);
                glClear(GL_DEPTH_BUFFER_BIT);
                for (auto entry : chunkData)
                {
                    auto currentChunk = entry.second;
                    std::shared_ptr<Renderer> renderer = currentChunk->GetRenderer();

                    // ShadowMap : First pass
                    shadowShader->Bind();
                    renderer->ChangeShader(shadowShader);
                    // Uniforms
                    lightPos = camera.GetPosition() + glm::vec3(0.0f, 10.0f, 0.0f) + lightDir * glm::vec3(-10.0);
                    lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0, 1.0, 0.0));
                    lightSpaceMatrix = lightProjection * lightView;
                    shadowShader->SetUniformMat4f("u_LightPV", lightSpaceMatrix);
                    texture.Bind(0);
                    renderer->Draw();
                }
            }
            else {
                glBindFramebuffer(GL_FRAMEBUFFER, DepthFBO);
                glClear(GL_DEPTH_BUFFER_BIT);
            }

            fbo.Bind();
            glViewport(0, 0, width, height);
            glm::vec3 backgroundColor = glm::mix(glm::vec3(0.67f, 0.90f, 0.90f), glm::vec3(1.0f, 0.8f, 0.3f), (glm::normalize(lightDir).y + 1.0f) * 0.5f);
            glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            //Uniforms
            model = glm::translate(glm::mat4{ 1.0 }, translation);
            view = camera.GetView();
            proj = camera.GetProjection();

            // Basic Shader
            shader->Bind();
            shader->SetUniform1i("u_ShadowMap", 1);
            shader->SetUniformMat4f("u_Model", model);
            shader->SetUniformMat4f("u_View", view);
            shader->SetUniformMat4f("u_Proj", proj);
            shader->SetUniform3f("u_CameraPos", camera.GetPosition());
            shader->SetUniformMat4f("u_LightPV", lightSpaceMatrix);
            shader->SetUniform3f("u_LightPos", lightPos);
            shader->SetUniform1i("show_Shadow", settings.Shadow);
            shader->SetUniform1i("show_PCF", settings.PCF);
            shader->SetUniform1i("show_PCSS", settings.PCSS);

            // BillBoard shader
            billBoardShader->Bind();
            billBoardShader->SetUniform1i("u_ShadowMap", 1);
            billBoardShader->SetUniformMat4f("u_Model", model);
            billBoardShader->SetUniformMat4f("u_View", view);
            billBoardShader->SetUniformMat4f("u_Proj", proj);
            billBoardShader->SetUniform3f("u_CameraPos", camera.GetPosition());
            billBoardShader->SetUniformMat4f("u_LightPV", lightSpaceMatrix);
            billBoardShader->SetUniform3f("u_LightPos", lightPos);
            billBoardShader->SetUniform1i("show_Shadow", settings.Shadow);
            billBoardShader->SetUniform1i("show_PCF", settings.PCF);
            billBoardShader->SetUniform1i("show_PCSS", settings.PCSS);

            // Water shader
            waterShader->Bind();
            waterShader->SetUniformMat4f("u_Model", model);
            waterShader->SetUniformMat4f("u_View", view);
            waterShader->SetUniformMat4f("u_Proj", proj);
            waterShader->SetUniform3f("u_CameraPos", camera.GetPosition());
            waterShader->SetUniform3f("u_LightPos", lightPos);
            waterShader->SetUniform1f("time", currentTime);
            waterShader->SetUniform1i("waterGeometry", settings.waterGeometry);

            // Frame Shader
            frameBufferShader->Bind();
            frameBufferShader->SetUniform1f("brightness", settings.brightness);
            frameBufferShader->SetUniform1f("contrast", settings.contrast);
            frameBufferShader->SetUniform1f("saturation", settings.saturation);
            frameBufferShader->SetUniform1f("gamma", settings.gamma);
            frameBufferShader->SetUniform1f("exposure", settings.exposure);
            frameBufferShader->SetUniform1i("foggy", settings.foggy);
            frameBufferShader->SetUniform1i("bloom", settings.bloom);
            frameBufferShader->SetUniform1i("reinhard", settings.reinhard);
            frameBufferShader->SetUniform1i("ce", settings.ce);
            frameBufferShader->SetUniform1i("filmic", settings.filmic);
            frameBufferShader->SetUniform1i("aces", settings.aces);

            // ShadowMap : Second pass
            for (auto entry : chunkData)
            {
                auto currentChunk = entry.second;
                std::shared_ptr<Renderer> renderer = currentChunk->GetRenderer();
                renderer->ChangeShader(allShaders);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, renderer->GetDepthMap());

                renderer->Draw();
            }
            for (auto entry : chunkData)
            {
                auto currentChunk = entry.second;
                std::shared_ptr<Renderer> renderer = currentChunk->GetRenderer();

                renderer->DrawWater();
            }

            if (settings.bloom)
                fbo.GaussianBlur(10, blurShader);
            frameBufferShader->Bind();
            if (world.GetBlockType(camera.GetPosition()) == BlockType::Water)
                frameBufferShader->SetUniform1i("underwater", 1);
            else
                frameBufferShader->SetUniform1i("underwater", 0);
            fbo.Render();
          
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Begin("Model Configs");
                ImGui::DragFloat3("Light Direction", glm::value_ptr(lightDir), 0.1f);
                ImGui::Checkbox("Shadow    ", &settings.Shadow);
                if (settings.Shadow)
                {
                    ImGui::SameLine();
                    ImGui::Checkbox("PCF   ", &settings.PCF);
                    if (settings.PCF)
                        settings.PCSS = false;
                    ImGui::SameLine();
                    ImGui::Checkbox("PCSS", &settings.PCSS);
                    if (settings.PCSS)
                        settings.PCF = false;
                }
                ImGui::DragInt("Render Distance", &renderDistance, 1, 1, 8);
                ImGui::DragFloat("Camera Speed", &settings.cameraSpeed, 0.1f, 0.0f, std::numeric_limits<float>::max());
                ImGui::Text("Camera Position: (%f, %f, %f)", 
                    camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
                ImGui::Checkbox("VSync", &settings.VSync);
                ImGui::Text("FPS: %.0f Hz", 1 / deltaTime);
                ImGui::Text("Rendering Time: %.0f ms", deltaTime * 1000);
                ImGui::Text("Loaded Chunks: %d", world.GetChunkData().size());
                ImGui::Checkbox("Geometry Shader Test", &settings.waterGeometry);
                ImGui::End();
            }
            {
                ImGui::Begin("Post Processing Settings");
                {
                    ImGui::Checkbox("Screen Space Fog", &settings.foggy);
                    ImGui::SameLine();
                    ImGui::Checkbox("Bloom", &settings.bloom);
                    ImGui::DragFloat("Brightness", &settings.brightness, 0.01f);
                    ImGui::DragFloat("Contrast", &settings.contrast, 0.01f);
                    ImGui::DragFloat("Saturation", &settings.saturation, 0.01f);
                    ImGui::DragFloat("Gamma Correction", &settings.gamma, 0.01f);
                    ImGui::Checkbox("Tone Mapping    ", &settings.toneMapping);
                    if (settings.toneMapping)
                    {
                        ImGui::Checkbox("Reinhard  ", &settings.reinhard);
                        if (settings.reinhard)
                        {
                            settings.aces = false;
                            settings.ce = false;
                            settings.filmic = false;
                        }
                        ImGui::SameLine();
                        ImGui::Checkbox("CryEngine  ", &settings.ce);
                        if (settings.ce)
                        {
                            settings.reinhard = false;
                            settings.filmic = false;
                            settings.aces = false;
                        }
                        ImGui::SameLine();
                        ImGui::Checkbox("Filmic", &settings.filmic);
                        if (settings.filmic)
                        {
                            settings.reinhard = false;
                            settings.ce = false;
                            settings.aces = false;
                        }
                        ImGui::SameLine();
                        ImGui::Checkbox("ACES", &settings.aces);
                        if (settings.aces)
                        {
                            settings.reinhard = false;
                            settings.ce = false;
                            settings.filmic = false;
                        }
                        ImGui::DragFloat("HDR Exposure", &settings.exposure, 0.01f);
                    }
                    else {
                        settings.reinhard = false;
                        settings.ce = false;
                        settings.filmic = false;
                        settings.aces = false;
                    }
                }
                ImGui::End();
            }
            {
                ImGui::Begin("Shadow Map Texture");
                ImGui::Image((void*)(intptr_t)DepthMapID, ImVec2(512, 512), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)); // 设定纹理显示大小
                ImGui::End();
            }
            {
                ImGui::Begin("Frame Buffer Texture");
                ImGui::Image((void*)(intptr_t)fbo.GetFBOTexture()[0], ImVec2(512, 288), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)); // 设定纹理显示大小
                ImGui::End();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    // Destructor need to be called AFTER vbo/ibo is deleted (fixed)
    }
    glfwTerminate();
    return 0;
}