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

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Chunk.h"

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    int width = 1280;
    int height = 720;
    window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    /* create a valid OpenGL rendering context and call glewInit() to initialize the extension entry points*/
    if (glewInit() != GLEW_OK)
        return -1;

    std::cout << "OpenGL Version:\t" << glGetString(GL_VERSION) << std::endl;

    /* ImGui Setup */
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    //GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //GLCall(glEnable(GL_BLEND));
    
    // Generate Terrains
    Chunk chunk(32);
    chunk.Generate(166615615154);
    std::vector<float> vtx = chunk.GetVertices();
    std::vector<unsigned int> idx = chunk.GetIndices();
   
    {
        // vertex array
        VertexArray va;
        va.Bind();

        // vertex buffer
        VertexBuffer vb(vtx.data(), vtx.size() * sizeof(float));

        // index buffer
        IndexBuffer ib(idx.data(), idx.size());

        VertexBufferLayout layout;
        layout.Push<float>(3);
        layout.Push<float>(3);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        //Load shader file
        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

        // Camera 
        Camera camera(45.0f, 0.1f, 100.0f, window);
        camera.OnResize(width, height);
        //camera.SetPosition(glm::vec3(0.0, 0.0, 0.0));
        
        //MVP matrix
        glm::mat4 model(1.0f);
        glm::vec3 translation{ 0.0f, 0.0f, 0.0f };
        model = glm::translate(model, translation);
        shader.SetUniformMat4f("u_Model", model);

        glm::mat4 view = camera.GetView();
        shader.SetUniformMat4f("u_View", view);

        //glm::mat4 proj2 = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        glm::mat4 proj = camera.GetProjection();
        shader.SetUniformMat4f("u_Proj", proj);

        // Uniforms
        glm::vec3 lightPos{ 100.0f,300.0f,200.0f };
        glm::vec3 lightDir{ -1.0f, -3.0f, -2.0f };
        shader.SetUniform3f("u_LightPos", lightPos);
        float lightIntensity = 3.0f;
        shader.SetUniform1f("u_LightIntensity", lightIntensity);
        shader.SetUniform3f("u_CameraPos", camera.GetPosition());
        float Kd = 1.0f; //diffuse K
        shader.SetUniform1f("u_Kd", Kd);
        float Ks = 1.0f; //specular K
        shader.SetUniform1f("u_Ks", Ks);

        // Texture
        Texture texture("res/textures/23w31a_blocks.png-atlas.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        // Unbind
        shader.Unbind();
        va.Unbind();
        vb.Unbind();
        ib.Unbind();

        Shader shadowShader("res/shaders/Shadow.shader");
        shadowShader.Bind();
        // Uniforms
        float near_plane = 1.0f, far_plane = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        shadowShader.SetUniformMat4f("u_Model", model);
        shadowShader.SetUniformMat4f("u_LightPV", lightSpaceMatrix);
        shadowShader.Unbind();

        Renderer renderer(&va, &ib, &shader);
        renderer.GenerateDepthMap();

        glEnable(GL_DEPTH_TEST);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Press Input */
            camera.OnUpdate(1.0f / 60.0f); // 60hz
            float currentTime = static_cast<float>(glfwGetTime());
            deltaTime = currentTime - lastFrameTime;
            lastFrameTime = currentTime;

            /* Render here */
            renderer.Clear();

            // ShadowMap : First pass
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, renderer.GetDepthMapFBO());
            glClear(GL_DEPTH_BUFFER_BIT);
            shadowShader.Bind();
            renderer.ChangeShader(&shadowShader);
            // Uniforms
            lightPos = camera.GetPosition() + glm::vec3(0.0f, 10.0f, 0.0f) + lightDir * glm::vec3(-10.0);
            lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0, 1.0, 0.0));
            lightSpaceMatrix = lightProjection * lightView;
            shadowShader.SetUniformMat4f("u_LightPV", lightSpaceMatrix);
            texture.Bind(0);
            renderer.Draw();
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // ShadowMap : Second pass
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shader.Bind();
            renderer.ChangeShader(&shader);
            texture.Bind(0);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, renderer.GetDepthMap());
            shader.SetUniform1i("u_ShadowMap", 1);

            //Uniforms
            model = glm::translate(glm::mat4{ 1.0 }, translation);
            shader.SetUniformMat4f("u_Model", model);

            view = camera.GetView();
            shader.SetUniformMat4f("u_View", view);

            proj = camera.GetProjection();
            shader.SetUniformMat4f("u_Proj", proj);

            shader.SetUniform3f("u_CameraPos", camera.GetPosition());
            shader.SetUniformMat4f("u_LightPV", lightSpaceMatrix);
            shader.SetUniform3f("u_LightPos", lightPos);

            renderer.Draw();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Begin("Model Configs");
                ImGui::DragFloat3("Light Direction", glm::value_ptr(lightDir), 0.1f);
                ImGui::Text("Camera Position: (%f, %f, %f)", 
                    camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
                ImGui::Text("FPS: %.0f Hz", 1 / deltaTime);
                ImGui::End();
            }
            {
                ImGui::Begin("Shadow Map Texture");
                ImGui::Image((void*)(intptr_t)renderer.GetDepthMap(), ImVec2(512, 512), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f)); // 设定纹理显示大小
                ImGui::End();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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