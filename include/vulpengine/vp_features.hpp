#pragma once

#ifdef __has_include

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

#if __has_include(<stb_include.h>)
#	define VP_HAS_STB_INCLUDE
#endif

#endif // __has_include