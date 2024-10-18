#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

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
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
         -0.5f, 0.5f,
          0.2f, -0.1f,
         -0.0f, 0.2f,
         -0.1f, 0.3f,
         -0.2f, 0.5f,
         -0.1f, 0.8f
    };

    unsigned int indices[] = {
         0,1,2,
         0,3,4,
         3,4,5,
         1,2,5
    };

    // vertex array
    VertexArray va;
    va.Bind();

    // vertex buffer
    VertexBuffer vb(positions, sizeof(positions));

    // index buffer
    IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    //Load shader file
    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    shader.Unbind();
    va.Unbind();
    vb.Unbind();
    ib.Unbind();

    Renderer renderer;

    float red = 0.8f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        shader.Bind();
        shader.SetUniform4f("u_Color", red, 0.3f, 0.8f, 1.0f);

        renderer.Draw(va, ib, shader);

        if (red > 1.0f)
            increment = -0.05f;
        else if (red < 0.0f)
            increment = 0.05f;

        red += increment;


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Need to be called AFTER vbo/ibo is deleted (unfixed)
    glfwTerminate();
    return 0;
}