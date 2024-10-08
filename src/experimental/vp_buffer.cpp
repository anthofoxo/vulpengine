#include "experimental/vp_buffer.hpp"

#include <cassert>

namespace vulpengine::experimental {
	Buffer::Buffer(CreateInfo const& info) {
		assert(info.content.size_bytes() > 0);

		glCreateBuffers(1, &mHandle);
		glNamedBufferStorage(mHandle, info.content.size_bytes(), info.content.data(), info.flags);

		if (!info.label.empty()) {
			glObjectLabel(GL_BUFFER, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());
		}
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Buffer::~Buffer() noexcept {
		if (mHandle) {
			glDeleteBuffers(1, &mHandle);
		}
	}
}