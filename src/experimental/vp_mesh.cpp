#include "experimental/vp_mesh.hpp"

namespace vulpengine::experimental {
	Mesh::Mesh(CreateInfo const& info) {
		mVertexArray = std::move(info.vertexArray);

		mBuffers.reserve(info.buffers.size());

		for (auto& buffer : info.buffers) {
			mBuffers.emplace_back(std::move(buffer.value));
		}

		mMode = info.mode;
		mCount = info.count;
		mType = info.type;
	}

	void Mesh::draw() const {
		mVertexArray.bind();
		if (mType != GL_NONE) glDrawElements(mMode, mCount, mType, nullptr);
		else glDrawArrays(mMode, 0, mCount);
	}
}