#if !defined(VP_WINDOWS) && !defined(VP_LINUX)
#	error Platform is not defined
#endif

#if !defined(VP_DEBUG) && !defined(VP_RELEASE) && !defined(VP_DIST)
#	error Configuration is not defined
#endif

#if defined(__has_include) && !__has_include(<glad/gl.h>)
#	error Vulpengine requires GLAD
#endif

#if defined(__has_include) && !__has_include(<GLFW/glfw3.h>)
#	error Vulpengine requires GLFW
#endif

#include <glad/gl.h>

#ifndef GL_VERSION_4_6
#	error OpenGL 4.6 is required, generate your glad files with at least OpenGL 4.6
#endif