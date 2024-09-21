#include "experimental/vp_texture.hpp"

#include "vp_log.hpp"

#include <cassert>

namespace vulpengine::experimental {
#ifdef VP_HAS_STB_IMAGE
	Texture::CreateInfo& Texture::CreateInfo::with_image(Image const& image) {
		target = GL_TEXTURE_2D;
		width = image.width();
		height = image.height();
		internalFormat = GL_RGBA8;
		return *this;
	}

	Texture::UploadInfo& Texture::UploadInfo::with_image(Image const& image) {
		xoffset = 0;
		yoffset = 0;
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

		glCreateTextures(info.target, 1, &mHandle);
		glTextureStorage2D(mHandle, 1, info.internalFormat, info.width, info.height);
		glTextureParameteri(mHandle, GL_TEXTURE_MIN_FILTER, info.minFilter);
		glTextureParameteri(mHandle, GL_TEXTURE_MAG_FILTER, info.magFilter);
		glTextureParameteri(mHandle, GL_TEXTURE_WRAP_S, info.wrap);
		glTextureParameteri(mHandle, GL_TEXTURE_WRAP_T, info.wrap);

		VP_LOG_TRACE("Created texture: {}", mHandle);
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

		glTextureSubImage2D(mHandle, 0, info.xoffset, info.yoffset, info.width, info.height, info.format, info.type, info.pixels);
	}

	void Texture::bind(GLuint unit) const {
		glBindTextureUnit(unit, mHandle);
	}
}