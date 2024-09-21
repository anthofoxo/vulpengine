#pragma once

#include <utility> // std::move

// Forward declare; Avoid including entire GLFW header
extern "C" typedef struct GLFWwindow GLFWwindow;

namespace vulpengine {
	class Window final {
	public:
		struct CreateInfo final {
			int width = 0, height = 0;
			char const* title = nullptr;
			bool maximized = false;
		};

		constexpr Window() noexcept = default;
		Window(CreateInfo const& info);
		Window(Window const&) = delete;
		Window& operator=(Window const&) = delete;
		inline Window(Window&& other) noexcept { *this = std::move(other); }
		Window& operator=(Window&& other) noexcept;
		~Window() noexcept;

		inline explicit operator bool() const { return mHandle != nullptr; }
		inline bool valid() const { return mHandle != nullptr; }
		inline GLFWwindow* handle() const { return mHandle; }

		bool should_close() const;
		void swap_buffers() const;
		void make_context_current() const;

		static void poll_events();
		static bool loadGl();
	private:
		GLFWwindow* mHandle = nullptr;
	};
}