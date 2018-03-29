#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

using namespace std;


int w = 1280, h = 960;/* Default window size */
GLFWwindow *window = NULL; /* Our window object */
double cx = -1.0, cy = -0.5, zoom = 0.25; /* Camera position and zoom value */
int itr = 100; /* Number of iterations */


float points[] = {
        -1.0f,  1.0f,  0.0f,
        -1.0f,  -1.0f,  0.0f,
        1.0f,  -1.0f,  0.0f,

        -1.0f,  1.0f,  0.0f,
        1.0f,  -1.0f,  0.0f,
        1.0f,  1.0f,  0.0f,
};


int main(int argc, char *argv[]) {
    if (!glfwInit()) {
        cerr << "Failed to init GLFW" << endl;
        return 1;
    }

    atexit(glfwTerminate); /* Always terminate GLFW when exiting */

    /* Sets the error-callback function, using lambda here for simplification */
    glfwSetErrorCallback([](int e, const char *s) { cerr << s << endl; });

    /* We want OpenGL 4.1, because only that version supports double-precision on the GPU */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(w, h, "Mandelbrot", NULL, NULL);
    if (!window) {
        cerr << "Failed to create window" << endl;
        return 1;
    }

    /* Lets us use OpenGL finally */
    glfwMakeContextCurrent(window);

    /* Init GLEW */
    //glewExperimental = GL_TRUE;
    //glewInit();

    /* Init GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    const char* vertex_shader =
            "#version 410\n"
                    "in vec3 vp;"
                    "void main () {"
                    "  gl_Position = vec4 (vp, 1.0);"
                    "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    std::ifstream t("../julia_shader.glsl");
    if(!t.is_open()) {
        cerr << "Cannot open mandelbrot_shader.glsl!" << endl;
        return 1;
    }
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    const char *src  = str.c_str();

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &src, NULL);
    glCompileShader (fs);

    int success;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
    if(!success) {
        int s;
        glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &s);

        char *buf = new char[s];
        glGetShaderInfoLog(fs, s, &s, buf);

        cerr << buf << endl;
        delete [] buf;
        return 1;
    }

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    glUseProgram(shader_program);

    GLuint vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 2 * 9 * sizeof (float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


    while(!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &w, &h);
        glUniform2d(glGetUniformLocation(shader_program, "screen_size"), (double)w, (double)h);
        glUniform1d(glGetUniformLocation(shader_program, "screen_ratio"), (double)w / (double)h);
        glUniform2d(glGetUniformLocation(shader_program, "center"), cx, cy);
        glUniform1d(glGetUniformLocation(shader_program, "zoom"), zoom);
        glUniform1i(glGetUniformLocation(shader_program, "itr"), itr);

        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray (vao);
        glDrawArrays (GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }


    glfwDestroyWindow(window);
}
