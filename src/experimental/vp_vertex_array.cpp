#include "experimental/vp_vertex_array.hpp"

#include <cassert>

namespace vulpengine::experimental {
	VertexArray::VertexArray(CreateInfo const& info) {
		glCreateVertexArrays(1, &mHandle);

		for (int i = 0; i < info.buffers.size(); ++i) {
			auto const& bufferinfo = info.buffers[i];
			assert(bufferinfo.buffer.valid());
			assert(bufferinfo.stride > 0);

			glVertexArrayVertexBuffer(mHandle, i, bufferinfo.buffer.handle(), bufferinfo.offset, bufferinfo.stride);
			glVertexArrayBindingDivisor(mHandle, i, bufferinfo.divisor);
		}

		for (int i = 0; i < info.attributes.size(); ++i) {
			auto const& attributeinfo = info.attributes[i];
			assert(attributeinfo.size > 0);
			assert(attributeinfo.type != GL_NONE);

			glEnableVertexArrayAttrib(mHandle, i);
			glVertexArrayAttribBinding(mHandle, i, attributeinfo.bindingindex);

			bool isFloatingPointType = false;
			switch (attributeinfo.type) {
			case GL_HALF_FLOAT:
			case GL_FLOAT:
			case GL_DOUBLE:
				isFloatingPointType = true;
				break;
			}

			if (isFloatingPointType)
				glVertexArrayAttribFormat(mHandle, i, attributeinfo.size, attributeinfo.type, GL_FALSE, attributeinfo.relativeoffset);
			else 
				glVertexArrayAttribIFormat(mHandle, i, attributeinfo.size, attributeinfo.type, attributeinfo.relativeoffset);
		}

		if (info.indexBuffer) {
			glVertexArrayElementBuffer(mHandle, info.indexBuffer->value.handle());
		}


		if (!info.label.empty()) {
			glObjectLabel(GL_VERTEX_ARRAY, mHandle, static_cast<GLsizei>(info.label.size()), info.label.data());
		}
	}

	VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
		std::swap(mHandle, other.mHandle);
		return *this;
	}

	VertexArray::~VertexArray() noexcept {
		if (mHandle) {
			glDeleteVertexArrays(1, &mHandle);
		}
	}

	void VertexArray::bind() const {
		assert(valid());
		glBindVertexArray(mHandle);
	}
}