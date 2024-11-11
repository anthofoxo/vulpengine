#pragma once

/*!
ShaderProgram is a simple type safe wrapper around an OpenGL shader program.

Needs Improvment:
1. OpenGL wrappers should be consolidated into one file. `vp_ogl` is likely the new target.

2. Needs more push_XXX api functions. The integer functions are a priority.

3. We should get better error messages, currently stb_include doesn't tell us
what was included or in what order, thus we can't reasonably match the source number
to a filepath. We should work on a custom stb_include like header.
*/

#include "vulpengine/vp_features.hpp"

#ifdef VP_HAS_STB_INCLUDE

#ifdef VP_HAS_GLM
#	include <glm/glm.hpp>
#endif

#include "vulpengine/vp_util.hpp"

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

		void push_1f(std::string_view name, float v0) const;
		void push_2f(std::string_view name, float const* v0) const;
		void push_2f(std::string_view name, float v0, float v1) const;
		void push_3f(std::string_view name, float const* v0) const;
		void push_3f(std::string_view name, float v0, float v1, float v2) const;
		void push_4f(std::string_view name, float const* v0) const;
		void push_4f(std::string_view name, float v0, float v1, float v2, float v3) const;

		void push_mat4f(std::string_view name, float const* v0) const;

#ifdef VP_HAS_GLM
		void push_2f(std::string_view name, glm::vec2 const& v0) const;
		void push_3f(std::string_view name, glm::vec3 const& v0) const;
		void push_4f(std::string_view name, glm::vec4 const& v0) const;

		void push_mat4f(std::string_view name, glm::mat4 const& v0) const;
#endif

		inline explicit operator bool() const { return mHandle; }
		inline operator GLuint() const { return mHandle; }
		inline bool valid() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
		UnorderedStringMap<int> mActiveUniforms;
	};
}
#endif // VP_HAS_STB_INCLUDE