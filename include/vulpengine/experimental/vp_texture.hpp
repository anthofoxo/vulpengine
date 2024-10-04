#pragma once

#include "vp_features.hpp"
#include "vp_image.hpp"

#include <glad/gl.h>

#include <array>

namespace vulpengine::experimental {
	class Texture final {
	public:
		struct CreateInfo final {
			GLenum target = GL_NONE;
			GLsizei width = 0, height = 0;
			GLenum internalFormat = GL_NONE;
			GLint minFilter = GL_LINEAR;
			GLint magFilter = GL_LINEAR;
			GLint wrap = GL_CLAMP_TO_EDGE;
			std::array<float, 4> border{ 1.0f, 1.0f, 1.0f, 1.0f };

#ifdef VP_HAS_STB_IMAGE
			CreateInfo& with_image(Image const& image);
#endif
		};

		struct UploadInfo final {
			GLint xoffset = 0, yoffset = 0;
			GLsizei width = 0, height = 0;
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

		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
	};
}