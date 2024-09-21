#pragma once

#if defined(__has_include) && __has_include(<stb_image.h>)
#	define VP_HAS_STB_IMAGE
#endif

#ifdef VP_HAS_STB_IMAGE

#include <utility>

namespace vulpengine::experimental {
	class Image final {
	public:
		constexpr Image() noexcept = default;
		Image(char const* filename);
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