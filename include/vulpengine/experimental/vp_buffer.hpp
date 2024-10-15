#pragma once

#include <glad/gl.h>

#include <utility> // std::move
#include <span>
#include <string_view>

namespace vulpengine::experimental {
	class Buffer final {
	public:
		struct CreateInfo final {
			std::span<std::byte const> content;
			GLbitfield flags = GL_NONE;
			std::string_view label;
		};

		constexpr Buffer() noexcept = default;
		Buffer(CreateInfo const& info);
		Buffer(Buffer const&) = delete;
		Buffer& operator=(Buffer const&) = delete;
		inline Buffer(Buffer&& other) noexcept { *this = std::move(other); }
		Buffer& operator=(Buffer&& other) noexcept;
		~Buffer() noexcept;

		void bind_base(GLenum target, GLuint index) const;

		void upload(GLintptr offset, std::span<std::byte const> data) const;
		void upload(GLintptr offset, GLsizeiptr size, void const* data) const;

		inline explicit operator bool() const { return mHandle; }
		inline bool valid() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
	};
}