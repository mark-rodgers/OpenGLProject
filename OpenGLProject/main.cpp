#define GLEW_STATIC

#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

SDL_Window* gWindow = NULL;
SDL_GLContext gContext;

void PrintKeyInfo(SDL_KeyboardEvent* key);

int main(int argc, char* args[]) {
    int quit = 0;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create Window
    gWindow = SDL_CreateWindow(
        "OpenGL Project",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if (gWindow == NULL) {
        printf("Window could not be created\nError: %s\n", SDL_GetError());
        getchar();
        return -1;
    }
    
    // Create Context
    gContext = SDL_GL_CreateContext(gWindow);
    if (gContext == NULL) {
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

    GLfloat vertices[] = {
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
        0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,
        0.0f, 0.5f * float(sqrt(3)) * 2 / 3, 0.0f,
    };

    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
        "}\0";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VAO, VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 1);
    glBindVertexArray(0);

    glClearColor(1.f, 1.f, 1.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(gWindow);

    // Application Loop
    SDL_Event event;
    while (!quit) {
        // Graphics
        glClearColor(1.f, 1.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        SDL_GL_SwapWindow(gWindow);

        // Input
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

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram); 

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