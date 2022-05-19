#include <SDL.h>
#include <glew.h>

#include <iostream>
#include <fstream>
#include <string>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

SDL_Window* gWindow = nullptr;
SDL_GLContext gContext = nullptr;

void PrintKeyInfo(SDL_KeyboardEvent* key);

static std::string LoadShaderSource(const std::string& filepath) {
	std::ifstream stream(filepath);

	std::string line;
	std::string shaderSource;
	while (getline(stream, line)) {
		shaderSource.append(line + "\n");
	}

	return shaderSource;
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(sizeof(char) * length);
		glGetShaderInfoLog(id, length, &length, message);

		printf("Failed to compile %s shader!\n", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"));
		printf("%s", message);
		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int main(int argc, char* args[]) {
	int quit = 0;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// Create Window
	gWindow = SDL_CreateWindow(
		"OpenGL Project",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);
	if (gWindow == nullptr) {
		printf("Window could not be created\nError: %s\n", SDL_GetError());
		getchar();
		return -1;
	}

	// Create Context
	gContext = SDL_GL_CreateContext(gWindow);
	if (gContext == nullptr) {
		printf("OpenGL context could not be created\nError: %s\n", SDL_GetError());
		getchar();
		return -1;
	}
	else {
		printf("OpenGL %s\n", glGetString(GL_VERSION));
	}

	// Initialize GLEW
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		printf("Failed to initialize GLEW\nError: %s\n", glewGetErrorString(glewError));
		getchar();
		return -1;
	}
	else {
		printf("GLEW %s initialized\n\n", glewGetString(GLEW_VERSION));
	}

	// Enable V-Sync
	if (SDL_GL_SetSwapInterval(-1) < 0) {
		printf("Error enabling adaptive vsync\n%s\n", SDL_GetError());
		if (SDL_GL_SetSwapInterval(1) < 0) {
			printf("Error enabling vsync\n%s\n", SDL_GetError());
		}
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	float positions[] = {
		-0.5f, -0.5f, // vertex index: 0
		 0.5f, -0.5f, // vertex index: 1
		 0.5f,  0.5f, // vertex index: 2
		-0.5f,  0.5f  // vertex index: 3
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 2, positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, indices, GL_STATIC_DRAW);

	std::string vertexShaderSource = LoadShaderSource("assets/shaders/FlatColor.shader.vert");
	std::string fragmentShaderSource = LoadShaderSource("assets/shaders/FlatColor.shader.frag");
	unsigned int shader = CreateShader(vertexShaderSource, fragmentShaderSource);
	glUseProgram(shader);

	// Application Loop
	SDL_Event event;
	while (!quit) {
		// GFX
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		// Swap front and back buffer
		SDL_GL_SwapWindow(gWindow);

		// INPUT
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				PrintKeyInfo(&event.key);
				if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = 1;
				break;
			case SDL_KEYUP:
				PrintKeyInfo(&event.key);
				break;
			case SDL_QUIT:
				printf("Program quit after %i ticks", event.quit.timestamp);
				quit = 1;
				break;
			}
		}
	}

	glDeleteProgram(shader);
	SDL_DestroyWindow(gWindow);
	SDL_Quit();
	return 0;
}

void PrintKeyInfo(SDL_KeyboardEvent* key) {
	if (key->type == SDL_KEYDOWN)
		printf("KEY DOWN - ");
	else
		printf("KEY UP   - ");

	printf("Scancode: 0x%02X", key->keysym.scancode);
	printf(", Name: %s\n", SDL_GetKeyName(key->keysym.sym));
}