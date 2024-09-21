#pragma once

#include "vp_features.hpp"

#ifdef VP_HAS_TRACY
#	include <glad/gl.h>
#	include <tracy/Tracy.hpp>
#	include <tracy/TracyOpenGL.hpp>
#	define VP_PROFILE_GPU_CONTEXT TracyGpuContext
#	define VP_PROFILE_CPU ZoneScoped
#	define VP_PROFILE_GPU TracyGpuZone(TracyFunction)
#	define VP_PROFILE_FRAME TracyGpuCollect; FrameMark
#else
#	define VP_PROFILE_GPU_CONTEXT
#	define VP_PROFILE_CPU
#	define VP_PROFILE_GPU
#	define VP_PROFILE_FRAME
#endif