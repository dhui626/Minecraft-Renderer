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


// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

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

    float positions[] = {
        // Vertex          // Normal           // Coords
        // 前面 (面 0)
        0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  25.0f / 64.0f, 23.0f / 32.0f, // 0
        1.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  26.0f / 64.0f, 23.0f / 32.0f, // 1
        1.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  26.0f / 64.0f, 24.0f / 32.0f, // 2
        0.0f, 1.0f, 0.0f,  0.0f, 0.0f, -1.0f,  25.0f / 64.0f, 24.0f / 32.0f, // 3
        // 后面 (面 1)
        0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,   25.0f / 64.0f, 23.0f / 32.0f, // 4
        1.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,   26.0f / 64.0f, 23.0f / 32.0f, // 5
        1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,   26.0f / 64.0f, 24.0f / 32.0f, // 6
        0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,   25.0f / 64.0f, 24.0f / 32.0f, // 7
        // 左侧 (面 2)
        0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  25.0f / 64.0f, 23.0f / 32.0f, // 4
        0.0f, 0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  26.0f / 64.0f, 23.0f / 32.0f, // 0
        0.0f, 1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,  26.0f / 64.0f, 24.0f / 32.0f, // 3
        0.0f, 1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,  25.0f / 64.0f, 24.0f / 32.0f, // 7
        // 右侧 (面 3)
        1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 0.0f,   25.0f / 64.0f, 23.0f / 32.0f, // 5
        1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,   26.0f / 64.0f, 23.0f / 32.0f, // 1
        1.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,   26.0f / 64.0f, 24.0f / 32.0f, // 2
        1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,   25.0f / 64.0f, 24.0f / 32.0f, // 6
        // 上面 (面 4)
        0.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,   11.0f / 64.0f, 14.0f / 32.0f, // 7
        1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,   12.0f / 64.0f, 14.0f / 32.0f, // 6
        1.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   12.0f / 64.0f, 15.0f / 32.0f, // 2
        0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,   11.0f / 64.0f, 15.0f / 32.0f, // 3
        // 下面 (面 5)
        0.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  21.0f / 64.0f, 18.0f / 32.0f, // 4
        1.0f, 0.0f, 1.0f,  0.0f, -1.0f, 0.0f,  22.0f / 64.0f, 18.0f / 32.0f, // 5
        1.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,  22.0f / 64.0f, 19.0f / 32.0f, // 1
        0.0f, 0.0f, 0.0f,  0.0f, -1.0f, 0.0f,  21.0f / 64.0f, 19.0f / 32.0f  // 0
    };

    unsigned int indices[] = {
        // 前面
        0, 1, 2,
        2, 3, 0,
        // 后面
        4, 5, 6,
        6, 7, 4,
        // 左侧
        8, 9, 10,
        10, 11, 8,
        // 右侧
        12, 13, 14,
        14, 15, 12,
        // 上面
        16, 17, 18,
        18, 19, 16,
        // 下面
        20, 21, 22,
        22, 23, 20
    };

    //GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //GLCall(glEnable(GL_BLEND));
    
    {
        // vertex array
        VertexArray va;
        va.Bind();

        // vertex buffer
        VertexBuffer vb(positions, sizeof(positions));

        // index buffer
        IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

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
        
        //MVP matrix
        glm::mat4 model(1.0f);
        //glm::vec3 translation{ 0.0f, 16.0f, 0.0f };
        glm::vec3 translation{ 0.0f, 0.0f, 0.0f };
        model = glm::translate(model, translation);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader.SetUniformMat4f("u_Model", model);

        glm::mat4 view = camera.GetView();
        shader.SetUniformMat4f("u_View", view);

        //glm::mat4 proj2 = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
        glm::mat4 proj = camera.GetProjection();
        shader.SetUniformMat4f("u_Proj", proj);

        // Uniforms
        glm::vec3 lightPos{ 20.0f,30.0f,20.0f };
        // This cube is light source
        //glm::vec3 lightPos = translation;
        shader.SetUniform3f("u_LightPos", lightPos);
        float lightIntensity = 30.0f;
        shader.SetUniform1f("u_LightIntensity", lightIntensity);
        shader.SetUniform3f("u_CameraPos", camera.GetPosition());
        float Kd = 1.0f; //diffuse K
        shader.SetUniform1f("u_Kd", Kd);
        float Ks = 1.0f; //specular K
        shader.SetUniform1f("u_Ks", Ks);

        // Texture
        Texture texture("res/textures/1.20.2_minecraft_textures_atlas_blocks_0.png");
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
        float near_plane = 0.1f, far_plane = 100.5f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        shadowShader.SetUniformMat4f("u_Model", model);
        shadowShader.SetUniformMat4f("u_LightPV", lightSpaceMatrix);
        shadowShader.Unbind();

        Renderer renderer(&va, &ib, &shader);
        renderer.GenerateDepthMap();

        // Generate Terrains
        Chunk chunk(16, &renderer);
        chunk.Generate();

        glEnable(GL_DEPTH_TEST);

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Press Input */
            camera.OnUpdate(1.0f / 30.0f); // 30hz

            /* Render here */
            renderer.Clear();

            // ShadowMap : First pass
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glBindFramebuffer(GL_FRAMEBUFFER, renderer.GetDepthMapFBO());
            glClear(GL_DEPTH_BUFFER_BIT);
            shadowShader.Bind();
            renderer.ChangeShader(&shadowShader);
            texture.Bind(0);
            //renderer.Draw();
            chunk.Render();
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
            //lightPos = translation;
            //shader.SetUniform3f("u_LightPos", lightPos);

            //renderer.Draw();
            chunk.Render();

            // render Depth map to quad for visual debugging
            // ---------------------------------------------
            Shader debugDepthQuad("res/shaders/Depth.shader");
            debugDepthQuad.Bind();
            debugDepthQuad.SetUniform1f("near_plane", near_plane);
            debugDepthQuad.SetUniform1f("far_plane", far_plane);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderer.GetDepthMap());
            //renderQuad();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            {
                ImGui::Begin("Model Configs");
                ImGui::DragFloat3("Light Position", glm::value_ptr(translation), 0.1f);
                ImGui::Text("Camera Position: (%f, %f, %f)", 
                    camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
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