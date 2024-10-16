#include "experimental/vp_shader_program.hpp"

#ifdef VP_HAS_STB_INCLUDE

#include "vp_log.hpp"

#include <stb_include.h>

#ifdef VP_HAS_GLM
#	include <glm/gtc/type_ptr.hpp>
#endif

#include <array>
#include <memory>
#include <format>
#include <cstdlib> // free

namespace {
	std::unique_ptr<char, decltype(&free)> preprocessShader(char const* file, char const* inject, char const* includePath) {
		char error[256]; // stb_include_file expects this to be 256
		memset(error, 0, sizeof(error));
#ifdef VP_FEATURE_STB_INCLUDE_COMPAT
		std::unique_ptr<char, decltype(&free)> source = { stb_include_file((char*)file, (char*)inject, (char*)includePath, error), &free };
#else
		std::unique_ptr<char, decltype(&free)> source = { stb_include_file(file, inject, includePath, error), &free };
#endif
		if (!source) VP_LOG_ERROR("Shader preprocessor error in {}: {}", file, error);
		return source;
	}

	class Shader final {
	public:
		struct CreateInfo {
			GLenum type = GL_NONE;
			std::string_view source;
			std::string_view label;
		};

		constexpr Shader() noexcept = default;

		Shader(CreateInfo const& info) {
			assert(info.type != GL_NONE);
			assert(info.source.size() > 0);

			GLchar const* string = info.source.data();
			GLint length = static_cast<GLint>(info.source.size());

			mHandle = glCreateShader(info.type);
			glShaderSource(mHandle, 1, &string, &length);
			glCompileShader(mHandle);

			GLint infoLogLength;
			glGetShaderiv(mHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

			GLint compileStatus;
			glGetShaderiv(mHandle, GL_COMPILE_STATUS, &compileStatus);

			if (infoLogLength > 0) {
				std::string infoLog;
				infoLog.resize(infoLogLength);
				glGetShaderInfoLog(mHandle, infoLogLength, nullptr, infoLog.data());

				if (compileStatus)
					VP_LOG_INFO("Shader info log in {}: {}", info.label, infoLog);
				else
					VP_LOG_ERROR("Shader compile error in {}: {}", info.label, infoLog);
			}

			if (!info.label.empty())
				glObjectLabel(GL_SHADER, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());

			VP_LOG_TRACE("Created shader: {}", mHandle);

			if (!compileStatus) {
				Shader::~Shader();
				mHandle = 0;
			}
		}

		Shader(Shader const&) = delete;
		Shader& operator=(Shader const&) = delete;
		inline Shader(Shader&& other) noexcept { *this = std::move(other); }
		inline Shader& operator=(Shader&& other) noexcept {
			std::swap(mHandle, other.mHandle);
			return *this;
		}

		inline ~Shader() noexcept {
			if (mHandle) {
				VP_LOG_TRACE("Deleted shader: {}", mHandle);
				glDeleteShader(mHandle);
			}
		}

		inline explicit operator bool() const { return mHandle; }
		inline bool valid() const { return mHandle; }
		inline GLuint handle() const { return mHandle; }
	private:
		GLuint mHandle = 0;
	};
}

namespace vulpengine::experimental {
	ShaderProgram::ShaderProgram(CreateInfo const& info) {
		assert(info.file != nullptr);

		std::unique_ptr vertSource = preprocessShader(info.file, "#version 460 core\n#define VERT", info.includePath);
		if (!vertSource) return;

		std::unique_ptr fragSource = preprocessShader(info.file, "#version 460 core\n#define FRAG",  info.includePath);
		if (!fragSource) return;

		std::string vertLabel = std::format("{} [vert]", info.file);
		std::string fragLabel = std::format("{} [frag]", info.file);

		Shader vert = {{
			.type = GL_VERTEX_SHADER,
			.source = vertSource.get(),
			.label = vertLabel
		}};

		if (!vert) return;

		Shader frag = {{
			.type = GL_FRAGMENT_SHADER,
			.source = fragSource.get(),
			.label = fragLabel
		}};

		if (!frag) return;

		mHandle = glCreateProgram();
		glAttachShader(mHandle, vert.handle());
		glAttachShader(mHandle, frag.handle());
		glLinkProgram(mHandle);
		glDetachShader(mHandle, vert.handle());
		glDetachShader(mHandle, frag.handle());

		GLint infoLogLength;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLint linkStatus;
		glGetProgramiv(mHandle, GL_LINK_STATUS, &linkStatus);

		if (infoLogLength > 0) {
			std::string infoLog;
			infoLog.resize(infoLogLength);
			glGetProgramInfoLog(mHandle, infoLogLength, nullptr, infoLog.data());

			if (linkStatus)
				VP_LOG_INFO("Program info log in {}: {}", info.file, infoLog);
			else
				VP_LOG_ERROR("Program link error in {}: {}", info.file, infoLog);
		}

		glObjectLabel(GL_PROGRAM, mHandle, -1, info.file);

		if (!linkStatus) {
			glDeleteProgram(mHandle);
			mHandle = 0;
		}

		if (mHandle)
			VP_LOG_TRACE("Created shader program: {}", mHandle);

		if (!mHandle) return;

		// Do introspection

		GLint activeUniforms;
		GLint maxUniformNameLength;
		glGetProgramInterfaceiv(mHandle, GL_UNIFORM, GL_ACTIVE_RESOURCES, &activeUniforms);
		glGetProgramInterfaceiv(mHandle, GL_UNIFORM, GL_MAX_NAME_LENGTH, &maxUniformNameLength);

		std::unique_ptr<char[]> uniformNameBuffer = std::make_unique<char[]>(maxUniformNameLength);

		for (int i = 0; i < activeUniforms; ++i) {
			GLsizei length;
			glGetProgramResourceName(mHandle, GL_UNIFORM, i, maxUniformNameLength, &length, uniformNameBuffer.get());
			std::string_view name(uniformNameBuffer.get(), length);

			std::array<GLenum, 1> properties = { GL_LOCATION };
			GLint location;
			glGetProgramResourceiv(mHandle, GL_UNIFORM, i, static_cast<GLsizei>(properties.size()), properties.data(), 1, nullptr, &location);

			if (location != -1) mActiveUniforms[std::string(name)] = location;
		}
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		std::swap(mActiveUniforms, other.mActiveUniforms);
		return *this;
	}

	ShaderProgram::~ShaderProgram() noexcept {
		if (mHandle) {
			VP_LOG_TRACE("Destroyed shader program: {}", mHandle);
			glDeleteProgram(mHandle);
		}
	}

	void ShaderProgram::bind() const {
		glUseProgram(mHandle);
	}

	GLint ShaderProgram::get_uniform_location(std::string_view name) const {
		auto it = mActiveUniforms.find(name);
		if (it == mActiveUniforms.end()) return -1;
		return it->second;
	}

	void ShaderProgram::push_1f(std::string_view name, float v0) const {
		glProgramUniform1f(mHandle, get_uniform_location(name), v0);
	}

	void ShaderProgram::push_2f(std::string_view name, float const* v0) const {
		glProgramUniform2fv(mHandle, get_uniform_location(name), 1, v0);
	}

	void ShaderProgram::push_2f(std::string_view name, float v0, float v1) const {
		glProgramUniform2f(mHandle, get_uniform_location(name), v0, v1);
	}

	void ShaderProgram::push_3f(std::string_view name, float const* v0) const {
		glProgramUniform3fv(mHandle, get_uniform_location(name), 1, v0);
	}

	void ShaderProgram::push_3f(std::string_view name, float v0, float v1, float v2) const {
		glProgramUniform3f(mHandle, get_uniform_location(name), v0, v1, v2);
	}

	void ShaderProgram::push_4f(std::string_view name, float const* v0) const {
		glProgramUniform4fv(mHandle, get_uniform_location(name), 1, v0);
	}

	void ShaderProgram::push_4f(std::string_view name, float v0, float v1, float v2, float v3) const {
		glProgramUniform4f(mHandle, get_uniform_location(name), v0, v1, v2, v3);
	}

	void ShaderProgram::push_mat4f(std::string_view name, float const* v0) const {
		glProgramUniformMatrix4fv(mHandle, get_uniform_location(name), 1, GL_FALSE, v0);
	}

#ifdef VP_HAS_GLM
	void ShaderProgram::push_2f(std::string_view name, glm::vec2 const& v0) const { push_2f(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_3f(std::string_view name, glm::vec3 const& v0) const { push_3f(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_4f(std::string_view name, glm::vec4 const& v0) const { push_4f(name, glm::value_ptr(v0)); }
	void ShaderProgram::push_mat4f(std::string_view name, glm::mat4 const& v0) const { push_mat4f(name, glm::value_ptr(v0)); }
#endif
}

#endif // VP_HAS_STB_INCLUDE