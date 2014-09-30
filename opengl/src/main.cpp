#define GLM_FORCE_RADIANS

#include <iostream>
#include <stdio.h>
#include "GL/gl3w.h"
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <png.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util/FileLoader.h"

using namespace std;

const char* vertexSource = 
    "#version 150 \n"
    "in vec2 position;"
    "in vec3 color;"
    "in vec2 texcoord;"
    "out vec3 _color;"
    "out vec2 _texcoord;"
    "uniform mat4 mView;"
    "void main() {"
    "_color = color;"
    "_texcoord = texcoord;"
    "gl_Position = mView * vec4(position, 0.0, 1.0);"
    "}";

const char* fragmentSource = 
    "#version 150 \n"
    "in vec3 _color;"
    "in vec2 _texcoord;"
    "out vec4 outColor;"
    "uniform sampler2D tex1;"
    "uniform sampler2D tex2;"
    "void main() {"
//    "outColor = texture(tex, _texcoord) * vec4(1.0, 0.0, 1.0, 1.0);"
//    "outColor = texture(tex, _texcoord) * vec4(_color, 1.0);"
    "vec4 color_tex1 = texture(tex1, _texcoord);"
    "vec4 color_tex2 = texture(tex2, _texcoord);"
    "outColor = mix(color_tex1, color_tex2, 0.5);"
//    "outColor = color_tex2;"
    "}";

png_byte* load_image(const char* filename, int &width, int &height) {
//    int width;
//    int height;

    //header for testing if it is a png
    png_byte header[8];

    //open file as binary
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        std::cerr << "image \"" << filename << "\" not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    //read the header
    int r = fread(header, 1, 8, fp);

    //test if png
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        fclose(fp);
        std::cerr << "image \"" << filename << "\" is not a png type." << std::endl;
        exit(EXIT_FAILURE);
    }

    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
            NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    //create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    //png error stuff, not sure libpng man suggests this.
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    //init png reading
    png_init_io(png_ptr, fp);

    //let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    //variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 twidth, theight;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
            NULL, NULL, NULL);

    //update width and height based on png info
    width = twidth;
    height = theight;

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte *image_data = new png_byte[rowbytes * height];
    if (!image_data) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    //row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep *row_pointers = new png_bytep[height];
    if (!row_pointers) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        delete[] image_data;
        fclose(fp);
        exit(EXIT_FAILURE);
    }
    // set the individual row_pointers to point at the correct offsets of image_data
    for (int i = 0; i < height; ++i)
        row_pointers[height - 1 - i] = image_data + i * rowbytes;

    //read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);
    bool alphaChannel = color_type == PNG_COLOR_TYPE_RGBA;

    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    delete[] row_pointers;
    fclose(fp);

    return image_data;
}

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
    //  pos(x,y)      color(r,g,b)      texture(x,y)
         0.5f,  1.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
         1.5f,  1.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
         1.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f
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
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
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
        cout << "__status: " << buffer << endl;
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
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colorAttrib = glGetAttribLocation(shaderProgram, "color");
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(colorAttrib);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(texAttrib);

    //-------------------TEXTURE STUFF-------------------------------------------
    int  width1 = 0, height1 = 0;
    int  width2 = 0, height2 = 0;
    png_byte* image1 = load_image("/home/paulocanedo/Pictures/wallpapers/test.png", width1, height1);
    png_byte* image2 = load_image("/home/paulocanedo/Pictures/wallpapers/test2.png", width2, height2);
    GLuint textures[2];
    glGenTextures(2, textures);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, image1);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex1"), 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    float pixels[] = {
            0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    };
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
    glUniform1i(glGetUniformLocation(shaderProgram, "tex2"), 1);
    glGenerateMipmap(GL_TEXTURE_2D);
    //---------------------------------------------------------------------------

    //-------------------MATRICES TRANSFORMATION---------------------------------
    glm::mat4 mView = glm::ortho(0.0f, 2.0f, 0.0f, 2.0f, -1.0f, 1.0f);
    GLint mViewPosition = glGetUniformLocation(shaderProgram, "mView");
    glUniformMatrix4fv(mViewPosition, 1, GL_FALSE, glm::value_ptr(mView));
    //---------------------------------------------------------------------------


    if(glGetError() != GL_NO_ERROR) {
        cout << ":: an error occuried on OpenGL ::" << endl;
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        float time = (float)clock() / (float)CLOCKS_PER_SEC;
        GLint color = glGetUniformLocation(shaderProgram, "color");
        // glUniform3f(color, (sin(time * 4.0f) + 1.0f) / 2.0f, 0.0f, 0.0f);

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