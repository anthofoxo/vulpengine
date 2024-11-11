#pragma once

/*!
A wrapper around an stb_image allocated image

Needs Improvment:
1. Needs more documentation.

2. Should look into support qoi images, this may require some changes to the spec.
*/

#include "vulpengine/vp_features.hpp"

#ifdef VP_HAS_STB_IMAGE

#include <utility>
#include <string>

namespace vulpengine::experimental {
	class Image final {
	public:
		constexpr Image() noexcept = default;
		Image(char const* filename, bool flip = true);
		Image(std::string const& filename, bool flip = true);
		Image(Image const&) = delete;
		Image& operator=(Image const&) = delete;
		inline Image(Image&& other) noexcept { *this = std::move(other); }
		Image& operator=(Image&& other) noexcept;
		~Image() noexcept;

		inline explicit operator bool() const { return mPixels; }
		inline bool valid() const { return mPixels; }
		inline int width() const { return mWidth; }
		inline int height() const { return mHeight; }
		inline void const* pixels() const { return mPixels; }
	private:
		int mWidth = 0;
		int mHeight = 0;
		void* mPixels = nullptr;
	};
}

#endif // VP_HAS_STB_IMAGE