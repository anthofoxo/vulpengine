#pragma once

#include "vp_features.hpp"

#ifdef VP_HAS_STB_INCLUDE

#include <glad/gl.h>

#include <utility>

namespace vulpengine::experimental {
	class ShaderProgram final {
	public:
		struct CreateInfo final {
			char const* file = nullptr;
			char const* includePath = "./";
		};

		constexpr ShaderProgram() noexcept = default;
		ShaderProgram(CreateInfo const& info);
		ShaderProgram(ShaderProgram const&) = delete;
		ShaderProgram& operator=(ShaderProgram const&) = delete;
		inline ShaderProgram(ShaderProgram&& other) noexcept { *this = std::move(other); }
		ShaderProgram& operator=(ShaderProgram&& other) noexcept;
		~ShaderProgram() noexcept;

		void bind() const;

		inline explicit operator bool() const { return mHandle; }
		inline bool valid() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
	};
}
#endif // VP_HAS_STB_INCLUDE