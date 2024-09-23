#include "vp_window.hpp"

#ifdef VP_HAS_GLFW

#include "vp_platform.hpp"
#include "vp_profile.hpp"
#include "vp_log.hpp"

// Prevent APIENTRY macro redefinition
#ifdef VP_WINDOWS
#	include <Windows.h>
#endif

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdint>

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

	bool Window::loadGl() {
		VP_PROFILE_CPU;
		return gladLoadGL(&glfwGetProcAddress);
	}
}
#endif // VP_HAS_GLFW