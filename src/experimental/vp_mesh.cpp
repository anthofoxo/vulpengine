#include "experimental/vp_mesh.hpp"

#include <cassert>

namespace vulpengine::experimental {
	Mesh::Mesh(CreateInfo const& info) : mMode(info.mode), mCount(info.count), mType(info.type) {
		assert(info.vertexArray.valid());
		assert(info.count > 0);

		mVertexArray = std::move(info.vertexArray);

		mBuffers.reserve(info.buffers.size());

		for (auto& buffer : info.buffers) {
			mBuffers.emplace_back(std::move(buffer.value));
		}
	}

	void Mesh::draw(GLsizei count) const {
		if (count == 0) count = mCount;

		mVertexArray.bind();
		if (mType != GL_NONE) glDrawElements(mMode, count, mType, nullptr);
		else glDrawArrays(mMode, 0, count);
	}
}