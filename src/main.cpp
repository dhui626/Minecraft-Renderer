#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"


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

    float positions[] = {
        // vertex          //coords
        // 前面 (面 0)
        0.0f, 0.0f, 0.0f,  25.0f / 64.0f, 23.0f / 32.0f, // 0
        1.0f, 0.0f, 0.0f,  26.0f / 64.0f, 23.0f / 32.0f, // 1
        1.0f, 1.0f, 0.0f,  26.0f / 64.0f, 24.0f / 32.0f, // 2
        0.0f, 1.0f, 0.0f,  25.0f / 64.0f, 24.0f / 32.0f, // 3
        // 后面 (面 1)
        0.0f, 0.0f, 1.0f,  25.0f / 64.0f, 23.0f / 32.0f, // 4
        1.0f, 0.0f, 1.0f,  26.0f / 64.0f, 23.0f / 32.0f, // 5
        1.0f, 1.0f, 1.0f,  26.0f / 64.0f, 24.0f / 32.0f, // 6
        0.0f, 1.0f, 1.0f,  25.0f / 64.0f, 24.0f / 32.0f, // 7
        // 左侧 (面 2)
        0.0f, 0.0f, 1.0f,  25.0f / 64.0f, 23.0f / 32.0f, // 4
        0.0f, 0.0f, 0.0f,  26.0f / 64.0f, 23.0f / 32.0f, // 0
        0.0f, 1.0f, 0.0f,  26.0f / 64.0f, 24.0f / 32.0f, // 3
        0.0f, 1.0f, 1.0f,  25.0f / 64.0f, 24.0f / 32.0f, // 7
        // 右侧 (面 3)
        1.0f, 0.0f, 1.0f,  25.0f / 64.0f, 23.0f / 32.0f, // 5
        1.0f, 0.0f, 0.0f,  26.0f / 64.0f, 23.0f / 32.0f, // 1
        1.0f, 1.0f, 0.0f,  26.0f / 64.0f, 24.0f / 32.0f, // 2
        1.0f, 1.0f, 1.0f,  25.0f / 64.0f, 24.0f / 32.0f, // 6
        // 上面 (面 4)
        0.0f, 1.0f, 1.0f,  11.0f / 64.0f, 14.0f / 32.0f, // 7
        1.0f, 1.0f, 1.0f,  12.0f / 64.0f, 14.0f / 32.0f, // 6
        1.0f, 1.0f, 0.0f,  12.0f / 64.0f, 15.0f / 32.0f, // 2
        0.0f, 1.0f, 0.0f,  11.0f / 64.0f, 15.0f / 32.0f, // 3
        // 下面 (面 5)
        0.0f, 0.0f, 1.0f,  21.0f / 64.0f, 18.0f / 32.0f, // 4
        1.0f, 0.0f, 1.0f,  22.0f / 64.0f, 18.0f / 32.0f, // 5
        1.0f, 0.0f, 0.0f,  22.0f / 64.0f, 19.0f / 32.0f, // 1
        0.0f, 0.0f, 0.0f,  21.0f / 64.0f, 19.0f / 32.0f  // 0
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

    // vertex array
    VertexArray va;
    va.Bind();

    // vertex buffer
    VertexBuffer vb(positions, sizeof(positions));

    // index buffer
    IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);
    //layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    //Load shader file
    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    // Camera 
    Camera camera(45.0f, 0.1f, 100.0f, window);
    camera.OnResize(width, height);

    //MVP matrix
    glm::mat4 model(1.0f);
    //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.SetUniformMat4f("u_Model", model);

    glm::mat4 view = camera.GetView();
    shader.SetUniformMat4f("u_View", view);

    //glm::mat4 proj2 = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    glm::mat4 proj = camera.GetProjection();
    shader.SetUniformMat4f("u_Proj", proj);


    Texture texture("res/textures/1.20.2_minecraft_textures_atlas_blocks_0.png");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);

    shader.Unbind();
    va.Unbind();
    vb.Unbind();
    ib.Unbind();

    Renderer renderer;

    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Press Input */
        camera.OnUpdate(1.0f / 30.0f); // 30hz

        /* Render here */
        renderer.Clear();

        shader.Bind();

        glm::mat4 model(1.0);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        shader.SetUniformMat4f("u_Model", model);

        view = camera.GetView();
        shader.SetUniformMat4f("u_View", view);

        proj = camera.GetProjection();
        shader.SetUniformMat4f("u_Proj", proj);

        renderer.Draw(va, ib, shader); 

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Need to be called AFTER vbo/ibo is deleted (unfixed)
    glfwTerminate();
    return 0;
}