#pragma once

#include "experimental/vp_wrap.hpp"
#include "experimental/vp_vertex_array.hpp"
#include "experimental/vp_buffer.hpp"

#include <vector>
#include <span>
#include <optional>

namespace vulpengine::experimental {
	class Mesh final {
	public:
		// If you're having trouble matching arguments, `count` typically needs explicitly cast to `GLsizei`
		struct CreateInfo final {
			VertexArray&& vertexArray;
			std::span<const Wrap<Buffer&&>> buffers;
			GLenum mode = GL_NONE;
			GLsizei count = 0;
			GLenum type = GL_NONE;
		};

		constexpr Mesh() noexcept = default;
		Mesh(CreateInfo const& info);

		void draw(GLsizei count = 0) const;

		inline bool valid() const { return mVertexArray.valid(); }
		inline GLsizei count() const { return mCount; }
	private:
		VertexArray mVertexArray;
		std::vector<Buffer> mBuffers;
		GLenum mMode = GL_NONE;
		GLsizei mCount = 0;
		GLenum mType = GL_NONE;
	};
}