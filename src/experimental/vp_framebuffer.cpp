#include "experimental/vp_framebuffer.hpp"

#include "vp_log.hpp"

#include <spdlog/spdlog.h>

namespace {
	template<class... Callable>
	struct Visitor : Callable... {
		using Callable::operator()...;
	};

#define VP_IMPL_EXPAND(x) case x: return #x
	std::string_view constant_to_string(GLuint val) {
		switch (val) {
		VP_IMPL_EXPAND(GL_DEBUG_SOURCE_API);
		VP_IMPL_EXPAND(GL_DEBUG_SOURCE_WINDOW_SYSTEM);
		VP_IMPL_EXPAND(GL_DEBUG_SOURCE_SHADER_COMPILER);
		VP_IMPL_EXPAND(GL_DEBUG_SOURCE_THIRD_PARTY);
		VP_IMPL_EXPAND(GL_DEBUG_SOURCE_APPLICATION);
		VP_IMPL_EXPAND(GL_DEBUG_SOURCE_OTHER);

		VP_IMPL_EXPAND(GL_DEBUG_TYPE_ERROR);
		VP_IMPL_EXPAND(GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR);
		VP_IMPL_EXPAND(GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR);
		VP_IMPL_EXPAND(GL_DEBUG_TYPE_PORTABILITY);
		VP_IMPL_EXPAND(GL_DEBUG_TYPE_PERFORMANCE);
		VP_IMPL_EXPAND(GL_DEBUG_TYPE_MARKER);
		VP_IMPL_EXPAND(GL_DEBUG_TYPE_OTHER);

		VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_NOTIFICATION);
		VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_LOW);
		VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_MEDIUM);
		VP_IMPL_EXPAND(GL_DEBUG_SEVERITY_HIGH);

		VP_IMPL_EXPAND(GL_FRAMEBUFFER_COMPLETE);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_UNDEFINED);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_UNSUPPORTED);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE);
		VP_IMPL_EXPAND(GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS);
		default: return "?";
		}
	}
#undef HE_IMPL_EXPAND
}

namespace vulpengine::experimental {
	Framebuffer::Framebuffer(CreateInfo const& info) {
		// Count attachments
		std::vector<GLenum> colorAttachments;
		colorAttachments.reserve(8);

		for (auto const& texrb : info.attachments) {
			auto attachment = texrb.attachment;

			if (attachment >= GL_COLOR_ATTACHMENT0 && attachment <= GL_COLOR_ATTACHMENT31)
				colorAttachments.push_back(attachment);
		}

		// If no color attachment, specifically use none
		if (colorAttachments.empty())
			colorAttachments.push_back(GL_NONE);

		glCreateFramebuffers(1, &mHandle);

		for (auto const& texrb : info.attachments) {
			auto attachment = texrb.attachment;

			std::visit(Visitor{
				[this, attachment](Texture& v) { glNamedFramebufferTexture(mHandle, attachment, v.handle(), 0); },
				[this, attachment](Renderbuffer& v) { glNamedFramebufferRenderbuffer(mHandle, attachment, GL_RENDERBUFFER, v.handle()); },
			}, texrb.source);
		}

		glNamedFramebufferDrawBuffers(mHandle, colorAttachments.size(), colorAttachments.data());

		auto status = glCheckNamedFramebufferStatus(mHandle, GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			VP_LOG_ERROR("Incomplete framebuffer: {}", constant_to_string(status));
		}
		
	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	Framebuffer::~Framebuffer() noexcept {
		if (mHandle)
			glDeleteFramebuffers(1, &mHandle);
	}

	void Framebuffer::bind() {
		glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	}
}