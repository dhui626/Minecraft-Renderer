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

    //float positions[] = {
    //     //position       //texture coord  //color
    //     -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, //0
    //      0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, //1
    //      0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, //2
    //     -0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, //3
    //};

    float positions[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.2f, 0.3f, 0.8f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.2f, 0.3f, 0.8f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.2f, 0.3f, 0.8f
    };

    unsigned int indices[] = {
         0,1,2,
         0,2,3
    };

    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    GLCall(glEnable(GL_BLEND));

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
    layout.Push<float>(3);
    va.AddBuffer(vb, layout);

    //Load shader file
    Shader shader("res/shaders/Basic.shader");
    shader.Bind();
    shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    //MVP matrix
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.SetUniformMat4f("u_Model", model);

    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    shader.SetUniformMat4f("u_View", view);

    //glm::mat4 proj2 = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
    shader.SetUniformMat4f("u_Proj", proj);

    glm::vec4 temp = proj * view * model * glm::vec4(0.5f, 0.5f, -1, 1);
    temp /= temp.a;

    Texture texture("res/textures/ChernoLogo2.png");
    texture.Bind();
    shader.SetUniform1i("u_Texture", 0);
    // second texture
    texture.AddTexture("res/textures/illust.jpg", 1);
    texture.Bind(1);
    shader.SetUniform1i("u_Texture2", 1);

    shader.Unbind();
    va.Unbind();
    vb.Unbind();
    ib.Unbind();

    Renderer renderer;

    float red = 0.8f;
    float increment = 0.05f;
    float green = 0.2f;
    float increment2 = 0.01f;

    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        shader.Bind();
        shader.SetUniform4f("u_Color", red, green, 0.8f, 1.0f);

        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        shader.SetUniformMat4f("u_Model", model);

        renderer.Draw(va, ib, shader); 

        if (red > 1.0f)
            increment = -0.05f;
        else if (red < 0.0f)
            increment = 0.05f;

        if (green > 1.0f)
            increment2 = -0.02f;
        else if (green < 0.0f)
            increment2 = 0.02f;

        red += increment;
        green += increment2;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Need to be called AFTER vbo/ibo is deleted (unfixed)
    glfwTerminate();
    return 0;
}