#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include "glad.h"
#include <GLFW/glfw3.h>
#include <sys/stat.h>
#include <cmath>
#include "stb_image.h"

static const double PI = 4*atan(4);

static void error_callback(int error, const char *description) {
	std::cerr << "GLFW Error: " << description << std::endl;
}

//std::string fragment_shader = "../mandelbrot_shader.glsl";
std::string fragment_shader = "../julia_shader.glsl";

double cx = 0.0, cy = 0.0, zoom = 0.5;
int exponent = 2;
double magnitude = 0.7885, direction = PI/2;
int itr = 100;
int fps = 0;

GLFWwindow *window = nullptr;

int w = 1920;
int h = 1080;

GLuint program;
GLuint shader;

double last_time = 0, current_time = 0;
unsigned int ticks = 0;

bool keys[1024] = { 0 };

static void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	double xr = 2.0 * (xpos / (double)w - 0.5);
	double yr = 2.0 * (ypos / (double)h - 0.5);

	if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		cx += (xr - cx) / zoom / 2.0;
		cy -= (yr - cy) / zoom / 2.0;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	zoom += yoffset * 0.1 * zoom;
	if(zoom < 0.1) {
		zoom = 0.1;
	}
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	const double d = 0.1 / zoom;

	if(action == GLFW_PRESS) {
		keys[key] = true;
	} else if(action == GLFW_RELEASE) {
		keys[key] = false;
	}

	if(keys[GLFW_KEY_ESCAPE]) {
		glfwSetWindowShouldClose(window, 1);
	} else if(keys[GLFW_KEY_A]) {
		cx -= d;
	} else if(keys[GLFW_KEY_D]) {
		cx += d;
	} else if(keys[GLFW_KEY_W]) {
		cy += d;
	} else if(keys[GLFW_KEY_S]) {
		cy -= d;
	} else if(keys[GLFW_KEY_MINUS]) {
		itr -= 10;
	} else if(keys[GLFW_KEY_EQUAL]) {
		itr += 10;
		if(itr <= 0) {
			itr = 0;
		}
	} else if(keys[GLFW_KEY_M]) {
		fragment_shader = "../mandelbrot_shader.glsl";
	} else if(keys[GLFW_KEY_J]) {
		fragment_shader = "../julia_shader.glsl";
	} else if(keys[GLFW_KEY_1]) {
        exponent = 2;
    } else if(keys[GLFW_KEY_2]) {
        exponent = 3;
    }else if(keys[GLFW_KEY_3]) {
        exponent = 4;
    }else if(keys[GLFW_KEY_4]) {
        exponent = 5;
    }else if(keys[GLFW_KEY_5]) {
        exponent = 6;
    }else if(keys[GLFW_KEY_6]) {
        exponent = 7;
    }else if(keys[GLFW_KEY_7]) {
        exponent = 8;
    }else if(keys[GLFW_KEY_8]) {
        exponent = 9;
    }else if(keys[GLFW_KEY_9]) {
        exponent = 12;
    }else if(keys[GLFW_KEY_0]) {
        exponent = 11;
    }else if(keys[GLFW_KEY_LEFT_BRACKET]) {
        direction -= 0.01;
    }else if(keys[GLFW_KEY_RIGHT_BRACKET]) {
        direction += 0.01;
    }else if(keys[GLFW_KEY_SEMICOLON]) {
		magnitude -= 0.002;
	}else if(keys[GLFW_KEY_APOSTROPHE]) {
		magnitude += 0.002;
	}

}

const char* vertex_shader =
"#version 410\n"
"in vec3 vp;"
"void main () {"
"  gl_Position = vec4 (vp, 1.0);"
"}";

const char* texture_vertex_shader =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"void main () {"
"  gl_Position = vec4 (aPos, 1.0);\n"
"  TexCoord = aTexCoord;\n"
"}";

const char* texture_fragment_shader =
	"#version 410\n"
	"out vec4 FragColor;\n"
	"in vec3 ourColor;\n"
	"in vec2 TexCoord;\n"
	"uniform sampler2D ourTexture;\n"
	"void main()\n"
	"{\n"
	"	FragColor = texture(ourTexture, TexCoord);\n"
	"}";

static void update_window_title()
{
	std::ostringstream ss;
	ss << "Mandelbrot Renderer (floatingwindow)";
	ss << ", FPS: " << fps;
	ss << ", Iterations: " << itr;
	ss << ", Zoom: " << zoom;
	ss << ", At: (" << std::setprecision(8) << cx << " + " << cy << "i)";
	glfwSetWindowTitle(window, ss.str().c_str());
}

static void compile_shader(GLuint &prog)
{
	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &vertex_shader, NULL);
	glCompileShader (vs);

	std::ifstream t(fragment_shader);
	if(!t.is_open()) {
		std::cerr << "Cannot open mandelbrot_shader.glsl!" << std::endl;
		return;
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

		std::cerr << buf << std::endl;
		delete [] buf;
		return;
	}

	prog = glCreateProgram ();
	glAttachShader (prog, fs);
	glAttachShader (prog, vs);
	glLinkProgram (prog);

	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if(!success) {
		int s;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &s);

		char *buf = new char[s];
		glGetProgramInfoLog(prog, s, &s, buf);

		std::cerr << buf << std::endl;
		delete [] buf;
		return;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

static void compile_texture_shader(GLuint &prog){
	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
	glShaderSource (vs, 1, &texture_vertex_shader, NULL);
	glCompileShader (vs);

	GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
	glShaderSource (fs, 1, &texture_fragment_shader, NULL);
	glCompileShader (fs);

	int success;
	glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
	if(!success) {
		int s;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &s);

		char *buf = new char[s];
		glGetShaderInfoLog(fs, s, &s, buf);

		std::cerr << buf << std::endl;
		delete [] buf;
		return;
	}

	prog = glCreateProgram ();
	glAttachShader (prog, fs);
	glAttachShader (prog, vs);
	glLinkProgram (prog);

	glGetProgramiv(prog, GL_LINK_STATUS, &success);
	if(!success) {
		int s;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &s);

		char *buf = new char[s];
		glGetProgramInfoLog(prog, s, &s, buf);

		std::cerr << buf << std::endl;
		delete [] buf;
		return;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

static time_t last_mtime;

static time_t get_mtime(const char *path)
{
	struct stat statbuf;
	if (stat(path, &statbuf) == -1) {
		perror(path);
		exit(1);
	}
	return statbuf.st_mtime;
}

int main(int argc, char *argv[])
{
	if(!glfwInit()) {
		std::cerr << "Failed to init GLFW" << std::endl;
		return 1;
	}

	atexit(glfwTerminate);

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//window = glfwCreateWindow(w, h, "Mandelbrot", glfwGetPrimaryMonitor()  , NULL); //switch 1st NULL to glfwGetPrimaryMonitor() for fullscreen
	window = glfwCreateWindow(w, h, "Mandelbrot", NULL, NULL); //switch 1st NULL to glfwGetPrimaryMonitor() for fullscreen
	if(!window) {
		std::cerr << "Failed to create window" << std::endl;
		return 1;
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

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

	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	GLuint prog;
	compile_shader(prog);
	GLuint texture_prog;
	compile_texture_shader(texture_prog);

	last_mtime = get_mtime(fragment_shader.c_str());

	float points[] = {
	   -1.0f,  1.0f,  0.0f, //top left
	   -1.0f,  -1.0f,  0.0f, //bottom left
	   1.0f,  -1.0f,  0.0f, //bottom right

	   -1.0f,  1.0f,  0.0f, //top left
	   1.0f,  -1.0f,  0.0f, //bottom right
	   1.0f,  1.0f,  0.0f, // top right
	};
    float texturePoints[] = {
			-1.0f,  1.0f,  0.0f, 	0.0f, 0.0f,	//top left
			-1.0f,  0.88f,  0.0f, 	0.0f, 1.0f,	//bottom left
			-0.75f,  0.88f,  0.0f, 	1.0f, 1.0f,//bottom right

			-1.0f,  1.0f,  0.0f, 	0.0f, 0.0f,//top left
			-0.75f,  0.88f,  0.0f, 	1.0f, 1.0f,//bottom right
			-0.75f,  1.0f,  0.0f, 	1.0f, 0.0f,// top right

	};
	unsigned int indices[] = {

	};

	GLuint* vbo = new GLuint[2];
	glGenBuffers (2, vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
	glBufferData (GL_ARRAY_BUFFER, 2 * 9 * sizeof (float), points, GL_STATIC_DRAW);
	glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
	glBufferData (GL_ARRAY_BUFFER, sizeof(texturePoints), texturePoints, GL_STATIC_DRAW);

	GLuint* vao = new GLuint[2];
	glGenVertexArrays (2, vao);
	glBindVertexArray (vao[0]);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray (vao[1]);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), NULL);
	glEnableVertexAttribArray (1);
	glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));

	glUseProgram (prog);

	last_time = glfwGetTime();

	glBindVertexArray (vao[0]);


	// 	load and create texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load("../Signature.png", &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);



	while(!glfwWindowShouldClose(window)) {

		glBindVertexArray(vao[0]);
		glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
		time_t new_time = get_mtime(fragment_shader.c_str());
		if(new_time != last_mtime) {
			glDeleteProgram(prog);
			compile_shader(prog);
			glUseProgram(prog);
			last_mtime = new_time;

			std::cout << "Reloaded shader: " << last_mtime << std::endl;
		}
		glUseProgram(prog);

		glfwGetWindowSize(window, &w, &h);
		glUniform2d(glGetUniformLocation(prog, "screen_size"), (double)w, (double)h);
		glUniform1d(glGetUniformLocation(prog, "screen_ratio"), (double)w / (double)h);
		glUniform2d(glGetUniformLocation(prog, "center"), cx, cy);
		glUniform1d(glGetUniformLocation(prog, "zoom"), zoom);
		glUniform1i(glGetUniformLocation(prog, "itr"), itr);
        glUniform2d(glGetUniformLocation(prog, "constant"), magnitude*cos(direction), magnitude*sin(direction));
        glUniform1i(glGetUniformLocation(prog, "exponent"), exponent);

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays (GL_TRIANGLES, 0, 6);

		glUseProgram(texture_prog);
		glBindVertexArray(vao[1]);
		glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);

		glDrawArrays (GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();

		ticks++;
		current_time = glfwGetTime();
		if(current_time - last_time > 1.0) {
			fps = ticks;
			update_window_title();
			last_time = glfwGetTime();
			ticks = 0;
		}
	}

	glfwDestroyWindow(window);
	delete[] vao;
	delete[] vbo;
}
