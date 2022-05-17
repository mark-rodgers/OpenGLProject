#define GLEW_STATIC

#include <iostream>
#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <gl/GLU.h>

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

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
        printf("GLEW %s initialized\n", glewGetString(GLEW_VERSION));
    }

    // Enable V-Sync
    if (SDL_GL_SetSwapInterval(-1) < 0) {
        printf("Error enabling adaptive vsync\n%s\n", SDL_GetError());
        if (SDL_GL_SetSwapInterval(1) < 0) {
            printf("Error enabling vsync\n%s\n", SDL_GetError());
        }
    }

    // TODO: Create and compile shaders here

    // Main Loop
    SDL_Event event;

    while (!quit) {
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
        SDL_GL_SwapWindow(gWindow);
    }

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