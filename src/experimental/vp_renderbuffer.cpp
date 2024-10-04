#include "experimental/vp_renderbuffer.hpp"

namespace vulpengine::experimental {
	Renderbuffer::Renderbuffer(CreateInfo const& info) {
		glCreateRenderbuffers(1, &mHandle);
		glNamedRenderbufferStorage(mHandle, info.internalFormat, info.width, info.height);
		
		if (!info.label.empty())
			glObjectLabel(GL_RENDERBUFFER, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());
	}

	Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Renderbuffer::~Renderbuffer() noexcept {
		if (mHandle)
			glDeleteRenderbuffers(1, &mHandle);
	}
}