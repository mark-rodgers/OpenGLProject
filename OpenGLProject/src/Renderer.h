#pragma once

#include <glew.h>

// TODO: rewrite this assert. __debugbreak() is compiler intrinsic
//       and should be rewritten in a non-platform specific way
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);