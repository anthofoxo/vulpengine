#pragma once

#include "vp_features.hpp"

#ifdef VP_HAS_STB_INCLUDE

#ifdef VP_HAS_GLM
#	include <glm/glm.hpp>
#endif

#include "vp_types.hpp"

#include <glad/gl.h>

#include <utility>

namespace vulpengine::experimental {
	class ShaderProgram final {
	public:
		struct CreateInfo final {
			char const* file = nullptr;
			char const* includePath = "./";
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

		void push_vec4f(std::string_view name, float const* v0) const;
		void push_vec4f(std::string_view name, float v0, float v1, float v2, float v3) const;

		void push_mat4f(std::string_view name, float const* v0) const;

#ifdef VP_HAS_GLM
		void push_vec4f(std::string_view name, glm::vec4 const& v0) const;

		void push_mat4f(std::string_view name, glm::mat4 const& v0) const;
#endif

		inline explicit operator bool() const { return mHandle; }
		inline bool valid() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
		UnorderedStringMap<int> mActiveUniforms;
	};
}
#endif // VP_HAS_STB_INCLUDE