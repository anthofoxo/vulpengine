#pragma once

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

// To correctly enable this feature the *const correct* version should be supplied.
// See the [pull request](https://github.com/nothings/stb/pull/1336) on stb.
// [This version](https://github.com/JuanDiegoMontoya/stb/blob/53a36967a8012590940122fc2e2117cc8f7eb34c/stb_include.h) is what we test against.
// If you are unable to provide the const correct version then you can define `VP_FEATURE_STB_INCLUDE_COMPAT` to apply fixes in the engine code.
#if __has_include(<stb_include.h>)
#	define VP_HAS_STB_INCLUDE
#endif

#endif // __has_include