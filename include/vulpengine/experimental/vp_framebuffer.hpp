#pragma once

#include <glad/gl.h>
#include <utility>

#include "vp_texture.hpp"
#include "vp_renderbuffer.hpp"

#include <variant>
#include <functional>
#include <span>

namespace vulpengine::experimental {
	class Framebuffer final {
	public:
		struct Attachment final {
			GLenum attachment;
			std::variant<std::reference_wrapper<Texture>, std::reference_wrapper<Renderbuffer>> source;
		};

		struct CreateInfo final {
			std::span<Attachment const> attachments;
		};

		constexpr Framebuffer() noexcept = default;
		Framebuffer(CreateInfo const& info);
		Framebuffer(Framebuffer const&) = delete;
		Framebuffer& operator=(Framebuffer const&) = delete;
		inline Framebuffer(Framebuffer&& other) noexcept { *this = std::move(other); }
		Framebuffer& operator=(Framebuffer&& other) noexcept;
		~Framebuffer() noexcept;

		inline GLuint handle() const { return mHandle; }

		void bind();
	private:
		GLuint mHandle = 0;
	};
}