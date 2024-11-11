#include "vulpengine/experimental/vp_image.hpp"

#ifdef VP_HAS_STB_IMAGE

#include "vulpengine/vp_log.hpp"

#include <stb_image.h>

namespace vulpengine::experimental {
	Image::Image(char const* filename, bool flip) {
		stbi_set_flip_vertically_on_load(flip);
		mPixels = stbi_load(filename, &mWidth, &mHeight, nullptr, 4);

		if (!mPixels) {
			VP_LOG_ERROR("{}", stbi_failure_reason());
		}
	}

	Image::Image(std::string const& filename, bool flip) {
		stbi_set_flip_vertically_on_load(flip);
		mPixels = stbi_load(filename.c_str(), &mWidth, &mHeight, nullptr, 4);

		if (!mPixels) {
			VP_LOG_ERROR("{}", stbi_failure_reason());
		}
	}

	Image& Image::operator=(Image&& other) noexcept {
		std::swap(mWidth, other.mWidth);
		std::swap(mHeight, other.mHeight);
		std::swap(mPixels, other.mPixels);
		return *this;
	}

	Image::~Image() noexcept {
		if (mPixels) {
			stbi_image_free(mPixels);
		}
	}
}
#endif // VP_HAS_STB_IMAGE