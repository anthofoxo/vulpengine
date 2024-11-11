#pragma once

/*!
Type safe OpenGL wrappers.
*/

#include "vulpengine/vp_features.hpp"
#include "vulpengine/experimental/vp_image.hpp"
#include "vulpengine/experimental/vp_wrap.hpp"

#include <glad/gl.h>

#ifdef VP_HAS_GLM
#	include <glm/glm.hpp>
#endif

#include <utility> // std::move
#include <span>
#include <string_view>
#include <array>
#include <optional>
#include <variant>
#include <functional>

namespace vulpengine::experimental {
	class Texture final {
	public:
		struct CreateInfo final {
			GLenum target = GL_NONE;
			GLsizei width = 0, height = 0, depth = 0;
			GLenum internalFormat = GL_NONE;
			GLint minFilter = GL_LINEAR;
			GLint magFilter = GL_LINEAR;
			GLint wrap = GL_CLAMP_TO_EDGE;
			std::array<float, 4> border{ 1.0f, 1.0f, 1.0f, 1.0f };
			std::string_view label;
			GLsizei levels = 1;
			GLfloat anisotropy = 1.0f;

#ifdef VP_HAS_STB_IMAGE
			CreateInfo& with_image(Image const& image);
#endif
		};

		struct UploadInfo final {
			GLint xoffset = 0, yoffset = 0, zoffset = 0;
			GLsizei width = 0, height = 0, depth = 0;
			GLenum format = GL_NONE;
			GLenum type = GL_NONE;
			void const* pixels = nullptr;

#ifdef VP_HAS_STB_IMAGE
			UploadInfo& with_image(Image const& image);
#endif
		};

		constexpr Texture() noexcept = default;
		Texture(CreateInfo const& info);
		Texture(Texture const&) = delete;
		Texture& operator=(Texture const&) = delete;
		inline Texture(Texture&& other) noexcept { *this = std::move(other); }
		Texture& operator=(Texture&& other) noexcept;
		~Texture() noexcept;

		void upload(UploadInfo const& info) const;
		void bind(GLuint unit) const;
		void generate_mips() const;

		inline operator GLuint() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
	};

	class Renderbuffer final {
	public:
		struct CreateInfo final {
			GLsizei width = 0, height = 0;
			GLenum internalFormat = GL_NONE;
			std::string_view label;
		};

		constexpr Renderbuffer() noexcept = default;
		Renderbuffer(CreateInfo const& info);
		Renderbuffer(Renderbuffer const&) = delete;
		Renderbuffer& operator=(Renderbuffer const&) = delete;
		inline Renderbuffer(Renderbuffer&& other) noexcept { *this = std::move(other); }
		Renderbuffer& operator=(Renderbuffer&& other) noexcept;
		~Renderbuffer() noexcept;

		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
	};
	
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
	
	class VertexArray final {
	public:
		struct AttributeInfo final {
			GLint size = 0;
			GLenum type = GL_NONE;
			GLuint relativeoffset = 0;
			GLuint bindingindex = 0;
			bool enabled = true;
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