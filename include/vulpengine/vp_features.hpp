#pragma once

/*! VulpEngine feature testing

VulpEngine optionally supports many libraries
If VulpEngine can see the headers, it'll enable them

Todo: Refactor feature macros:
- VP_LIB_<library_name> should be used to indicate if a library is available
- VP_HAS_<vulpengine_type> should be used to indicate if a specific structure is available
*/

#ifdef __has_include

#if __has_include(<GLFW/glfw3.h>)
#	define VP_HAS_GLFW
#endif

#if __has_include(<glm/glm.hpp>)
#	define VP_HAS_GLM
#endif

#if __has_include(<tracy/Tracy.hpp>)
#	define VP_HAS_TRACY
#endif

#if __has_include(<spdlog/spdlog.h>)
#	define VP_HAS_SPDLOG
#endif

#if __has_include(<stb_image.h>)
#	define VP_HAS_STB_IMAGE
#endif

#if __has_include(<glad/gl.h>)
#	define VP_LIB_GLAD
#endif

#endif // __has_include