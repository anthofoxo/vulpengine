#include "experimental/vp_texture.hpp"

#include "vp_log.hpp"

#include <cassert>

#ifdef VP_HAS_GLM
#	include <glm/glm.hpp>
#endif

namespace vulpengine::experimental {
#ifdef VP_HAS_STB_IMAGE
	Texture::CreateInfo& Texture::CreateInfo::with_image(Image const& image) {
		target = GL_TEXTURE_2D;
		width = image.width();
		height = image.height();
		internalFormat = GL_RGBA8;
#ifdef VP_HAS_GLM
		levels = static_cast<int>(glm::ceil(glm::log2(glm::max(static_cast<float>(width), static_cast<float>(height))))) + 1;
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

		if(info.depth > 0)
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
			glObjectLabel(GL_TEXTURE, mHandle, info.label.size(), info.label.data());
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