#pragma once

#include "experimental/vp_wrap.hpp"
#include "experimental/vp_buffer.hpp"

#include <glad/gl.h>

#include <utility>
#include <span>
#include <optional>
#include <string_view>

namespace vulpengine::experimental {
	class VertexArray final {
	public:
		struct AttributeInfo final {
			GLint size = 0;
			GLenum type = GL_NONE;
			GLuint relativeoffset = 0;
			GLuint bindingindex = 0;
		};

		struct BufferInfo final {
			Buffer const& buffer;
			GLintptr offset = 0;
			GLsizei stride = 0;
			GLuint divisor = 0;
		};

		struct CreateInfo final {
			std::span<const BufferInfo> buffers;
			std::span<const AttributeInfo> attributes;
			std::optional<Wrap<Buffer const&>> indexBuffer;
			std::string_view label;
		};

		constexpr VertexArray() noexcept = default;
		VertexArray(CreateInfo const& info);
		VertexArray(VertexArray const&) = delete;
		VertexArray& operator=(VertexArray const&) = delete;
		inline VertexArray(VertexArray&& other) noexcept { *this = std::move(other); }
		VertexArray& operator=(VertexArray&& other) noexcept;
		~VertexArray() noexcept;

		void bind() const;

		inline explicit operator bool() const { return mHandle; }
		inline bool valid() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
	};
}