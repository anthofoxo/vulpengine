#include "vulpengine/vp_window.hpp"

#ifdef VP_HAS_GLFW

#include "vulpengine/vp_platform.hpp"
#include "vulpengine/vp_profile.hpp"
#include "vulpengine/vp_log.hpp"

// Prevent APIENTRY macro redefinition
#ifdef VP_WINDOWS
#	include <Windows.h>
#endif

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdint>

#define VP_MAKE_VERSION(major, minor, revision) (major * 1000 + minor * 100 + revision)

#if VP_MAKE_VERSION(GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION) < 3400
#	error GLFW 3.4+ is required
#endif

namespace {
	uint_fast16_t gWindowCount = 0;

	void errorCallback(int error, char const* description) {
		VP_LOG_ERROR("GLFW Error {}: {}", error, description);
	}
}

namespace vulpengine {
	Window::Window(CreateInfo const& info) {
		VP_PROFILE_CPU;

		if (!gWindowCount) {
			glfwSetErrorCallback(&errorCallback);

			if (is_wsl())
				glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);

			if (!glfwInit()) return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef VP_DIST
		glfwWindowHint(GLFW_CONTEXT_DEBUG, GLFW_TRUE);
#endif

		glfwWindowHint(GLFW_MAXIMIZED, info.maximized);

		mHandle = glfwCreateWindow(info.width, info.height, info.title, nullptr, nullptr);

		if (!mHandle && !gWindowCount) {
			glfwTerminate();
			return;
		}

		++gWindowCount;
		VP_LOG_DEBUG("Created window: {}", static_cast<void*>(mHandle));
	}

	Window& Window::operator=(Window&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Window::~Window() noexcept {
		if (mHandle) {
			VP_LOG_DEBUG("Destroyed window: {}", static_cast<void*>(mHandle));
			glfwMakeContextCurrent(nullptr);
			glfwDestroyWindow(mHandle);

			if (!--gWindowCount)
				glfwTerminate();
		}
	}

	bool Window::should_close() const {
		return glfwWindowShouldClose(mHandle);
	}

	void Window::swap_buffers() const {
		VP_PROFILE_CPU;
		VP_PROFILE_GPU;
		glfwSwapBuffers(mHandle);
	}

	void Window::make_context_current() const {
		glfwMakeContextCurrent(mHandle);
	}

	void Window::poll_events() {
		VP_PROFILE_CPU;
		glfwPollEvents();
	}

	bool Window::load_gl() {
		VP_PROFILE_CPU;
		return gladLoadGL(&glfwGetProcAddress);
	}
}
#endif // VP_HAS_GLFW