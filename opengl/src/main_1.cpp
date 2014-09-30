#include <iostream>
#include <stdio.h>
#include <math.h>

#include "GL/gl3w.h"
#include <GLFW/glfw3.h>

#include "util/FileLoader.h"

using namespace std;

const char* vertexSource = "#version 150 \n in vec2 position; in vec3 color; out vec3 _color; void main() {gl_Position = vec4(position, 0.0, 1.0); _color = color;}";
const char* fragmentSource = "#version 150 \n in vec3 _color; out vec4 outColor; void main() {outColor = vec4(_color, 1.0);}";

int main() {
	GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (gl3wInit()) {
        fprintf(stderr, "failed to initialize OpenGL\n");
        return -1;
    }

    if (!gl3wIsSupported(3, 2)) {
        fprintf(stderr, "OpenGL 3.2 not supported\n");
        return -1;
    }
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    // string vfilename("src/shaders/vshader.vsh");
    // string ffilename("src/shaders/fshader.fsh");
    // const char* vertexSource   = FileLoader::getFileContents(vfilename).c_str();
    // const char* fragmentSource = FileLoader::getFileContents(ffilename).c_str();

    float vertices[] = {
        -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,  
         0.5f,  0.5f, 0.0f, 1.0f, 0.0f,  
         0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 1.0f, 1.0f, 1.0f 
    };

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //-------------------VERTEX SHADER-------------------------------------------
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        cout << "--------------------------\n"
         << buffer
         << "--------------------------\n"
         << endl;
    }
    //-------------------FRAGMENT SHADER-----------------------------------------
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
        cout << "--------------------------\n"
         << buffer
         << "--------------------------\n"
         << endl;
    }
    //---------------------------------------------------------------------------
    //-------------------CREATE AND LINK PROGRAM---------------------------------
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    //---------------------------------------------------------------------------

    GLuint shaders[5];
    GLsizei count;
    GLint programStatus;
    glGetAttachedShaders(shaderProgram, 5, &count, shaders);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programStatus);
    // printf("vertexShader: %d\n", vertexShader);
    // printf("fragmentShader: %d\n", fragmentShader);
    // printf("count: %d\n", count);
    // printf("shader[0]: %d, shader[1]: %d\n", shaders[0], shaders[1]);
    // printf("shaderProgram: %d\n", shaderProgram);
    // printf("programStatus: %s\n", programStatus == GL_TRUE ? "true" : "false");
    if(programStatus != GL_TRUE) {
        char buffer[512];
        glGetProgramInfoLog(shaderProgram, 512, &count, buffer);
        cout << buffer << endl;
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    //-------------------ELEMENT ARRAY UPLOAD------------------------------------
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    //---------------------------------------------------------------------------

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colorAttrib = glGetAttribLocation(shaderProgram, "color");
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(colorAttrib);

    if(glGetError() != GL_NO_ERROR) {
        cout << "an error occuried on OpenGL" << endl;
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        float time = (float)clock() / (float)CLOCKS_PER_SEC;
        GLint color = glGetUniformLocation(shaderProgram, "color");
        glUniform3f(color, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}