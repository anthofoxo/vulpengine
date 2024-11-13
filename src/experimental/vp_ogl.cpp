#include "vulpengine/vp_log.hpp"
#include "vulpengine/vp_util.hpp"
#include "vulpengine/experimental/vp_ogl.hpp"

#include <stb_include.h>

#ifdef VP_HAS_GLM
#	include <glm/gtc/type_ptr.hpp>
#endif

#include <cassert>
#include <array>
#include <memory>
#include <format>
#include <cstdlib>

namespace {
#define VP_IMPL_EXPAND(x) case x: return #x
	std::string_view constant_to_string(GLuint val) {
		switch (val) {
			VP_IMPL_EXPAND(GL_DEBUG_SOURCE_API);
			VP_IMPL_EXPAND(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
			VP_IMPL_EXPAND(GL_DEBUG_SOURCE_SHADER_COMPILER);
			VP_IMPL_EXPAND(GL_DEBUG_SOURCE_THIRD_PARTY);
			VP_IMPL_EXPAND(GL_DEBUG_SOURCE_APPLICATION);
			VP_IMPL_EXPAND(GL_DEBUG_SOURCE_OTHER);

			VP_IMPL_EXPAND(GL_DEBUG_TYPE_ERROR);
			VP_IMPL_EXPAND(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
			VP_IMPL_EXPAND(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
			VP_IMPL_EXPAND(GL_DEBUG_TYPE_PORTABILITY);
			VP_IMPL_EXPAND(GL_DEBUG_TYPE_PERFORMANCE);
			VP_IMPL_EXPAND(GL_DEBUG_TYPE_MARKER);
			VP_IMPL_EXPAND(GL_DEBUG_TYPE_OTHER);

			VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_NOTIFICATION);
			VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_LOW);
			VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_MEDIUM);
			VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_HIGH);

			VP_IMPL_EXPAND(GL_FRAMEBUFFER_COMPLETE);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_UNDEFINED);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_UNSUPPORTED);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
			VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
		default: return "?";
		}
	}
#undef HE_IMPL_EXPAND
}

// Buffer
namespace vulpengine::experimental {
	Buffer::Buffer(CreateInfo const& info) {
		assert(info.content.size_bytes() > 0);

		glCreateBuffers(1, &mHandle);
		glNamedBufferStorage(mHandle, info.content.size_bytes(), info.content.data(), info.flags);

		if (!info.label.empty()) {
			glObjectLabel(GL_BUFFER, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());
		}
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Buffer::~Buffer() noexcept {
		if (mHandle) {
			glDeleteBuffers(1, &mHandle);
		}
	}

	void Buffer::bind_base(GLenum target, GLuint index) const {
		glBindBufferBase(target, index, mHandle);
	}

	void Buffer::upload(GLintptr offset, std::span<std::byte const> data) const {
		upload(offset, data.size_bytes(), data.data());
	}

	void Buffer::upload(GLintptr offset, GLsizeiptr size, void const* data) const {
		glNamedBufferSubData(mHandle, offset, size, data);
	}
}

// Framebuffer
namespace vulpengine::experimental {
	Framebuffer::Framebuffer(CreateInfo const& info) {
		// Count attachments
		std::vector<GLenum> colorAttachments;
		colorAttachments.reserve(8);

		for (auto const& texrb : info.attachments) {
			auto attachment = texrb.attachment;

			if (attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT31)
				colorAttachments.push_back(attachment);
		}

		// If no color attachment, specifically use none
		if (colorAttachments.empty())
			colorAttachments.push_back(GL_NONE);

		glCreateFramebuffers(1, &mHandle);

		for (auto const& texrb : info.attachments) {
			auto attachment = texrb.attachment;

			std::visit(Visitor{
				[this, attachment](Texture& v) { glNamedFramebufferTexture(mHandle, attachment, v.handle(), 0); },
				[this, attachment](Renderbuffer& v) { glNamedFramebufferRenderbuffer(mHandle, attachment, GL_RENDERBUFFER, v.handle()); },
			}, texrb.source);
		}

		glNamedFramebufferDrawBuffers(mHandle, static_cast<GLsizei>(colorAttachments.size()), colorAttachments.data());

		auto status = glCheckNamedFramebufferStatus(mHandle, GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			VP_LOG_ERROR("Incomplete framebuffer: {}", constant_to_string(status));
		}

	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Framebuffer::~Framebuffer() noexcept {
		if (mHandle)
			glDeleteFramebuffers(1, &mHandle);
	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	}
}

// Renderbuffer
namespace vulpengine::experimental {
	Renderbuffer::Renderbuffer(CreateInfo const& info) {
		assert(info.width > 0);
		assert(info.height > 0);
		assert(info.internalFormat != GL_NONE);

		glCreateRenderbuffers(1, &mHandle);
		glNamedRenderbufferStorage(mHandle, info.internalFormat, info.width, info.height);

		if (!info.label.empty())
			glObjectLabel(GL_RENDERBUFFER, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());
	}

	Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Renderbuffer::~Renderbuffer() noexcept {
		if (mHandle)
			glDeleteRenderbuffers(1, &mHandle);
	}
}

// Texture
namespace vulpengine::experimental {
	namespace {
#if defined(GL_VERSION_4_6) || defined(GL_ARB_texture_filter_anisotropic)
#	define VP_GL_MAX_TEXTURE_MAX_ANISOTROPY GL_MAX_TEXTURE_MAX_ANISOTROPY
#	define VP_GL_TEXTURE_MAX_ANISOTROPY GL_TEXTURE_MAX_ANISOTROPY
#endif

#if !defined(VP_GL_MAX_TEXTURE_MAX_ANISOTROPY) && defined(GL_EXT_texture_filter_anisotropic)
#	define VP_GL_MAX_TEXTURE_MAX_ANISOTROPY GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT
#	define VP_GL_TEXTURE_MAX_ANISOTROPY GL_TEXTURE_MAX_ANISOTROPY_EXT
#endif

		bool has_anisotropy() {
#if defined(GL_VERSION_4_6)
			if (GLAD_GL_VERSION_4_6) {
				return true;
			}
#elif defined(GL_ARB_texture_filter_anisotropic)
			if (GLAD_GL_ARB_texture_filter_anisotropic) {
				return true;
			}
#elif defined(GL_EXT_texture_filter_anisotropic)
			if (GLAD_GL_EXT_texture_filter_anisotropic) {
				return true;
			}
#endif
			return false;
		}

		float max_anisotropy() {
#if defined(GL_VERSION_4_6)
			if (GLAD_GL_VERSION_4_6) {
				float v;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &v);
				return v;
			}
#elif defined(GL_ARB_texture_filter_anisotropic)
			if (GLAD_GL_ARB_texture_filter_anisotropic) {
				float v;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &v);
				return v;
			}
#elif defined(GL_EXT_texture_filter_anisotropic)
			if (GLAD_GL_EXT_texture_filter_anisotropic) {
				float v;
				glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &v);
				return v;
			}
#endif
			return 1.0f;
		}
	}

#ifdef VP_HAS_STB_IMAGE
	Texture::CreateInfo& Texture::CreateInfo::with_image(Image const& image) {
		target = GL_TEXTURE_2D;
		width = image.width();
		height = image.height();
		internalFormat = GL_RGBA8;
#ifdef VP_HAS_GLM
		levels = static_cast<int>(glm::floor(glm::log2(glm::max(static_cast<float>(width), static_cast<float>(height))))) + 1;
#endif
		return *this;
	}

	Texture::UploadInfo& Texture::UploadInfo::with_image(Image const& image) {
		width = image.width();
		height = image.height();
		format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		pixels = image.pixels();
		return *this;
	}
#endif

	Texture::Texture(CreateInfo const& info) {
		assert(info.target != GL_NONE);
		assert(info.width > 0);
		assert(info.height > 0);
		assert(info.internalFormat != GL_NONE);
		assert(info.levels > 0);

		glCreateTextures(info.target, 1, &mHandle);

		if (info.depth > 0)
			glTextureStorage3D(mHandle, info.levels, info.internalFormat, info.width, info.height, info.depth);
		else
			glTextureStorage2D(mHandle, info.levels, info.internalFormat, info.width, info.height);

		glTextureParameteri(mHandle, GL_TEXTURE_MIN_FILTER, info.minFilter);
		glTextureParameteri(mHandle, GL_TEXTURE_MAG_FILTER, info.magFilter);
		glTextureParameteri(mHandle, GL_TEXTURE_WRAP_S, info.wrap);
		glTextureParameteri(mHandle, GL_TEXTURE_WRAP_T, info.wrap);
		glTextureParameteri(mHandle, GL_TEXTURE_WRAP_R, info.wrap);
		glTextureParameteri(mHandle, GL_TEXTURE_MAX_LEVEL, info.levels - 1);
		glTextureParameterfv(mHandle, GL_TEXTURE_BORDER_COLOR, info.border.data());

		if (has_anisotropy()) {
			float maxAnisotropy = max_anisotropy();
			glTextureParameterf(mHandle, VP_GL_TEXTURE_MAX_ANISOTROPY, info.anisotropy > maxAnisotropy ? maxAnisotropy : info.anisotropy);
		}

		VP_LOG_TRACE("Created texture: {}", mHandle);

		if (!info.label.empty())
			glObjectLabel(GL_TEXTURE, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());
	}

	Texture& Texture::operator=(Texture&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Texture::~Texture() noexcept {
		if (mHandle) {
			VP_LOG_TRACE("Destroyed texture: {}", mHandle);
			glDeleteTextures(1, &mHandle);
		}
	}

	void Texture::upload(UploadInfo const& info) const {
		assert(info.width > 0);
		assert(info.height > 0);
		assert(info.format != GL_NONE);
		assert(info.type != GL_NONE);

		if (info.depth > 0)
			glTextureSubImage3D(mHandle, 0, info.xoffset, info.yoffset, info.zoffset, info.width, info.height, info.depth, info.format, info.type, info.pixels);
		else
			glTextureSubImage2D(mHandle, 0, info.xoffset, info.yoffset, info.width, info.height, info.format, info.type, info.pixels);
	}

	void Texture::bind(GLuint unit) const {
		glBindTextureUnit(unit, mHandle);
	}

	void Texture::generate_mips() const {
		glGenerateTextureMipmap(mHandle);
	}
}

// Vertex Array
namespace vulpengine::experimental {
	VertexArray::VertexArray(CreateInfo const& info) {
		glCreateVertexArrays(1, &mHandle);

		for (int i = 0; i < info.buffers.size(); ++i) {
			auto const& bufferinfo = info.buffers[i];
			assert(bufferinfo.buffer.valid());
			assert(bufferinfo.stride > 0);

			glVertexArrayVertexBuffer(mHandle, i, bufferinfo.buffer.handle(), bufferinfo.offset, bufferinfo.stride);
			glVertexArrayBindingDivisor(mHandle, i, bufferinfo.divisor);
		}

		for (int i = 0; i < info.attributes.size(); ++i) {
			auto const& attributeinfo = info.attributes[i];
			assert(attributeinfo.size > 0);
			assert(attributeinfo.type != GL_NONE);

			if (attributeinfo.enabled) glEnableVertexArrayAttrib(mHandle, i);
			glVertexArrayAttribBinding(mHandle, i, attributeinfo.bindingindex);

			bool isFloatingPointType = false;
			switch (attributeinfo.type) {
			case GL_HALF_FLOAT:
			case GL_FLOAT:
			case GL_DOUBLE:
				isFloatingPointType = true;
				break;
			}

			if (isFloatingPointType)
				glVertexArrayAttribFormat(mHandle, i, attributeinfo.size, attributeinfo.type, GL_FALSE, attributeinfo.relativeoffset);
			else
				glVertexArrayAttribIFormat(mHandle, i, attributeinfo.size, attributeinfo.type, attributeinfo.relativeoffset);
		}

		if (info.indexBuffer) {
			glVertexArrayElementBuffer(mHandle, info.indexBuffer->value.handle());
		}


		if (!info.label.empty()) {
			glObjectLabel(GL_VERTEX_ARRAY, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());
		}
	}

	VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	VertexArray::~VertexArray() noexcept {
		if (mHandle) {
			glDeleteVertexArrays(1, &mHandle);
		}
	}

	void VertexArray::bind() const {
		assert(valid());
		glBindVertexArray(mHandle);
	}
}

#ifdef VP_HAS_SHADER_PROGRAM
namespace vulpengine::experimental {
	namespace {
		std::unique_ptr<char, decltype(&free)> preprocessShader(char const* file, char const* inject, char const* includePath) {
			char error[256]; // stb_include_file expects this to be 256
			memset(error, 0, sizeof(error));
			std::unique_ptr<char, decltype(&free)> source = { stb_include_file((char*)file, (char*)inject, (char*)includePath, error), &free };
			if (!source) VP_LOG_ERROR("Shader preprocessor error in {}: {}", file, error);
			return source;
		}

		class Shader final {
		public:
			struct CreateInfo {
				GLenum type = GL_NONE;
				std::string_view source;
				std::string_view label;
			};

			constexpr Shader() noexcept = default;

			Shader(CreateInfo const& info) {
				assert(info.type != GL_NONE);
				assert(info.source.size() > 0);

				GLchar const* string = info.source.data();
				GLint length = static_cast<GLint>(info.source.size());

				mHandle = glCreateShader(info.type);
				glShaderSource(mHandle, 1, &string, &length);
				glCompileShader(mHandle);

				GLint infoLogLength;
				glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

				GLint compileStatus;
				glGetShaderiv(mHandle, GL_COMPILE_STATUS, &compileStatus);

				if (infoLogLength > 0) {
					std::string infoLog;
					infoLog.resize(infoLogLength);
					glGetShaderInfoLog(mHandle, infoLogLength, nullptr, infoLog.data());

					if (compileStatus)
						VP_LOG_INFO("Shader info log in {}: {}", info.label, infoLog);
					else
						VP_LOG_ERROR("Shader compile error in {}: {}", info.label, infoLog);
				}

				if (!info.label.empty())
					glObjectLabel(GL_SHADER, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());

				VP_LOG_TRACE("Created shader: {}", mHandle);

				if (!compileStatus) {
					Shader::~Shader();
					mHandle = 0;
				}
			}

			Shader(Shader const&) = delete;
			Shader& operator=(Shader const&) = delete;
			inline Shader(Shader&& other) noexcept { *this = std::move(other); }
			inline Shader& operator=(Shader&& other) noexcept {
				std::swap(mHandle, other.mHandle);
				return *this;
			}

			inline ~Shader() noexcept {
				if (mHandle) {
					VP_LOG_TRACE("Deleted shader: {}", mHandle);
					glDeleteShader(mHandle);
				}
			}

			inline explicit operator bool() const { return mHandle; }
			inline bool valid() const { return mHandle; }
			inline GLuint handle() const { return mHandle; }
		private:
			GLuint mHandle = 0;
		};
	}

	ShaderProgram::ShaderProgram(CreateInfo const& info) {
		assert(info.file != nullptr);

		std::unique_ptr vertSource = preprocessShader(info.file, "#version 460 core\n#define VERT", info.includePath);
		if (!vertSource) return;

		std::unique_ptr fragSource = preprocessShader(info.file, "#version 460 core\n#define FRAG", info.includePath);
		if (!fragSource) return;

		std::string vertLabel = std::format("{} [vert]", info.file);
		std::string fragLabel = std::format("{} [frag]", info.file);

		Shader vert = { {
			.type = GL_VERTEX_SHADER,
			.source = vertSource.get(),
			.label = vertLabel
		} };

		if (!vert) return;

		Shader frag = { {
			.type = GL_FRAGMENT_SHADER,
			.source = fragSource.get(),
			.label = fragLabel
		} };

		if (!frag) return;

		mHandle = glCreateProgram();
		glAttachShader(mHandle, vert.handle());
		glAttachShader(mHandle, frag.handle());
		glLinkProgram(mHandle);
		glDetachShader(mHandle, vert.handle());
		glDetachShader(mHandle, frag.handle());

		GLint infoLogLength;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLint linkStatus;
		glGetProgramiv(mHandle, GL_LINK_STATUS, &linkStatus);

		if (infoLogLength > 0) {
			std::string infoLog;
			infoLog.resize(infoLogLength);
			glGetProgramInfoLog(mHandle, infoLogLength, nullptr, infoLog.data());

			if (linkStatus)
				VP_LOG_INFO("Program info log in {}: {}", info.file, infoLog);
			else
				VP_LOG_ERROR("Program link error in {}: {}", info.file, infoLog);
		}

		glObjectLabel(GL_PROGRAM, mHandle, -1, info.file);

		if (!linkStatus) {
			glDeleteProgram(mHandle);
			mHandle = 0;
		}

		if (mHandle)
			VP_LOG_TRACE("Created shader program: {}", mHandle);

		if (!mHandle) return;

		// Do introspection

		GLint activeUniforms;
		GLint maxUniformNameLength;
		glGetProgramInterfaceiv(mHandle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &activeUniforms);
		glGetProgramInterfaceiv(mHandle, GL_UNIFORM, GL_MAX_NAME_LENGTH, &maxUniformNameLength);

		std::unique_ptr<char[]> uniformNameBuffer = std::make_unique<char[]>(maxUniformNameLength);

		for (int i = 0; i < activeUniforms; ++i) {
			GLsizei length;
			glGetProgramResourceName(mHandle, GL_UNIFORM, i, maxUniformNameLength, &length, uniformNameBuffer.get());
			std::string_view name(uniformNameBuffer.get(), length);

			std::array<GLenum, 1> properties = { GL_LOCATION };
			GLint location;
			glGetProgramResourceiv(mHandle, GL_UNIFORM, i, static_cast<GLsizei>(properties.size()), properties.data(), 1, nullptr, &location);

			if (location != -1) mActiveUniforms[std::string(name)] = location;
		}
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		std::swap(mActiveUniforms, other.mActiveUniforms);
		return *this;
	}

	ShaderProgram::~ShaderProgram() noexcept {
		if (mHandle) {
			VP_LOG_TRACE("Destroyed shader program: {}", mHandle);
			glDeleteProgram(mHandle);
		}
	}

	void ShaderProgram::bind() const {
		glUseProgram(mHandle);
	}

	GLint ShaderProgram::get_uniform_location(std::string_view name) const {
		auto it = mActiveUniforms.find(name);
		if (it == mActiveUniforms.end()) return -1;
		return it->second;
	}

	void ShaderProgram::push_1i(std::string_view name, int const* v0, int count) const {
		glProgramUniform1iv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_1i(std::string_view name, int v0) const {
		glProgramUniform1i(mHandle, get_uniform_location(name), v0);
	}

	void ShaderProgram::push_2i(std::string_view name, int const* v0, int count) const {
		glProgramUniform2iv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_2i(std::string_view name, int v0, int v1) const {
		glProgramUniform2i(mHandle, get_uniform_location(name), v0, v1);
	}

	void ShaderProgram::push_3i(std::string_view name, int const* v0, int count) const {
		glProgramUniform3iv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_3i(std::string_view name, int v0, int v1, int v2) const {
		glProgramUniform3i(mHandle, get_uniform_location(name), v0, v1, v2);
	}

	void ShaderProgram::push_4i(std::string_view name, int const* v0, int count) const {
		glProgramUniform4iv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_4i(std::string_view name, int v0, int v1, int v2, int v3) const {
		glProgramUniform4i(mHandle, get_uniform_location(name), v0, v1, v2, v3);
	}

	void ShaderProgram::push_1f(std::string_view name, float const* v0, int count) const {
		glProgramUniform1fv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_1f(std::string_view name, float v0) const {
		glProgramUniform1f(mHandle, get_uniform_location(name), v0);
	}

	void ShaderProgram::push_2f(std::string_view name, float const* v0, int count) const {
		glProgramUniform2fv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_2f(std::string_view name, float v0, float v1) const {
		glProgramUniform2f(mHandle, get_uniform_location(name), v0, v1);
	}

	void ShaderProgram::push_3f(std::string_view name, float const* v0, int count) const {
		glProgramUniform3fv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_3f(std::string_view name, float v0, float v1, float v2) const {
		glProgramUniform3f(mHandle, get_uniform_location(name), v0, v1, v2);
	}

	void ShaderProgram::push_4f(std::string_view name, float const* v0, int count) const {
		glProgramUniform4fv(mHandle, get_uniform_location(name), count, v0);
	}

	void ShaderProgram::push_4f(std::string_view name, float v0, float v1, float v2, float v3) const {
		glProgramUniform4f(mHandle, get_uniform_location(name), v0, v1, v2, v3);
	}

	void ShaderProgram::push_mat4f(std::string_view name, float const* v0) const {
		glProgramUniformMatrix4fv(mHandle, get_uniform_location(name), 1, GL_FALSE, v0);
	}

#ifdef VP_HAS_GLM
	void ShaderProgram::push_2i(std::string_view name, glm::ivec2 const& v0) const { push_2i(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_3i(std::string_view name, glm::ivec3 const& v0) const { push_3i(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_4i(std::string_view name, glm::ivec4 const& v0) const { push_4i(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_2f(std::string_view name, glm::vec2 const& v0) const { push_2f(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_3f(std::string_view name, glm::vec3 const& v0) const { push_3f(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_4f(std::string_view name, glm::vec4 const& v0) const { push_4f(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_mat4f(std::string_view name, glm::mat4 const& v0) const { push_mat4f(name, glm::value_ptr(v0)); }
#endif
}
#endif // VP_HAS_SHADER_PROGRAM