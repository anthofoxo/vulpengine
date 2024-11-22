#pragma once

/*!
Type safe OpenGL wrappers.
*/

#include "vulpengine/vp_features.hpp"
#include "vulpengine/vp_util.hpp"
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

	class ShaderProgram final {
	public:
		struct CreateInfo final {
			char const* path = nullptr;
		};

		ShaderProgram() noexcept = default;
		ShaderProgram(CreateInfo const& info);
		ShaderProgram(ShaderProgram const&) = delete;
		ShaderProgram& operator=(ShaderProgram const&) = delete;
		inline ShaderProgram(ShaderProgram&& other) noexcept { *this = std::move(other); }
		ShaderProgram& operator=(ShaderProgram&& other) noexcept;
		~ShaderProgram() noexcept;

		void bind() const;
		GLint get_uniform_location(std::string_view name) const;

		void push_1i(std::string_view name, int v0) const;
		void push_1i(std::string_view name, int const* v0, int count = 1) const;
		inline void push_1i(std::string_view name, std::span<int const> v0) const { push_1i(name, v0.data(), static_cast<int>(v0.size())); }

		void push_2i(std::string_view name, int v0, int v1) const;
		void push_2i(std::string_view name, int const* v0, int count = 1) const;

		void push_3i(std::string_view name, int v0, int v1, int v2) const;
		void push_3i(std::string_view name, int const* v0, int count = 1) const;

		void push_4i(std::string_view name, int v0, int v1, int v2, int v3) const;
		void push_4i(std::string_view name, int const* v0, int count = 1) const;

		void push_1f(std::string_view name, float v0) const;
		void push_1f(std::string_view name, float const* v0, int count = 1) const;
		inline void push_1f(std::string_view name, std::span<float const> v0) const { push_1f(name, v0.data(), static_cast<int>(v0.size())); }

		void push_2f(std::string_view name, float v0, float v1) const;
		void push_2f(std::string_view name, float const* v0, int count = 1) const;

		void push_3f(std::string_view name, float v0, float v1, float v2) const;
		void push_3f(std::string_view name, float const* v0, int count = 1) const;

		void push_4f(std::string_view name, float v0, float v1, float v2, float v3) const;
		void push_4f(std::string_view name, float const* v0, int count = 1) const;

		void push_mat4f(std::string_view name, float const* v0) const;
#ifdef VP_HAS_GLM
		inline void push_2i(std::string_view name, glm::ivec2 const* v0, int count = 1) const { push_2i(name, reinterpret_cast<int const*>(v0), count); }
		inline void push_3i(std::string_view name, glm::ivec3 const* v0, int count = 1) const { push_3i(name, reinterpret_cast<int const*>(v0), count); }
		inline void push_4i(std::string_view name, glm::ivec4 const* v0, int count = 1) const { push_4i(name, reinterpret_cast<int const*>(v0), count); }
		inline void push_2f(std::string_view name, glm::vec2 const* v0, int count = 1) const { push_2f(name, reinterpret_cast<float const*>(v0), count); }
		inline void push_3f(std::string_view name, glm::vec3 const* v0, int count = 1) const { push_3f(name, reinterpret_cast<float const*>(v0), count); }
		inline void push_4f(std::string_view name, glm::vec4 const* v0, int count = 1) const { push_4f(name, reinterpret_cast<float const*>(v0), count); }
		inline void push_2i(std::string_view name, std::span<glm::ivec2 const> v0) const { push_2i(name, v0.data(), static_cast<int>(v0.size())); }
		inline void push_3i(std::string_view name, std::span<glm::ivec3 const> v0) const { push_3i(name, v0.data(), static_cast<int>(v0.size())); }
		inline void push_4i(std::string_view name, std::span<glm::ivec4 const> v0) const { push_4i(name, v0.data(), static_cast<int>(v0.size())); }
		inline void push_2f(std::string_view name, std::span<glm::vec2 const> v0) const { push_2f(name, v0.data(), static_cast<int>(v0.size())); }
		inline void push_3f(std::string_view name, std::span<glm::vec3 const> v0) const { push_3f(name, v0.data(), static_cast<int>(v0.size())); }
		inline void push_4f(std::string_view name, std::span<glm::vec4 const> v0) const { push_4f(name, v0.data(), static_cast<int>(v0.size())); }
		void push_2i(std::string_view name, glm::ivec2 const& v0) const;
		void push_3i(std::string_view name, glm::ivec3 const& v0) const;
		void push_4i(std::string_view name, glm::ivec4 const& v0) const;
		void push_2f(std::string_view name, glm::vec2 const& v0) const;
		void push_3f(std::string_view name, glm::vec3 const& v0) const;
		void push_4f(std::string_view name, glm::vec4 const& v0) const;
		void push_mat4f(std::string_view name, glm::mat4 const& v0) const;
#endif

		inline explicit operator bool() const { return mHandle; }
		inline operator GLuint() const { return mHandle; }
		inline bool valid() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
		UnorderedStringMap<int> mActiveUniforms;
	};
}