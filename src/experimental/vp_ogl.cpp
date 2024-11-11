#include "vulpengine/vp_log.hpp"
#include "vulpengine/vp_util.hpp"
#include "vulpengine/experimental/vp_ogl.hpp"

#include <cassert>

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
		glTextureParameterf(mHandle, GL_TEXTURE_MAX_ANISOTROPY, info.anisotropy);
		glTextureParameterfv(mHandle, GL_TEXTURE_BORDER_COLOR, info.border.data());

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