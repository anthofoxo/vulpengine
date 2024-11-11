#pragma once

/*!
vp_stream is an experimental wrapper around a vector
the general idea is to allow us to push raw data into a vector reguardless of type
you can then take the span and directly use the byte data
Intended purpose is for use with graphics api uploading

All data is shallow copied, direct support for containers and arrays are not implemented yet
A general purpose write function is provided, given a data pointer and size
*/

#include <vector>
#include <span>
#include <type_traits>

namespace vulpengine::experimental {
	struct ByteStream {
		// Shallow copy
		template<class T, std::enable_if_t<std::is_pod_v<T>, int> = 0>
		void write(T const& val) {
			for (size_t i = 0; i < sizeof(T); ++i) mData.emplace_back();
			*reinterpret_cast<T*>(mData.data() + (mData.size() - sizeof(T))) = val;
		}

		// Allows generic containers to be written if they have the following:
		// `size() const` member function
		// `data() const` member function
		// `value_type` using declaration
		//
		// std::vector, std::array, std::span, etc match these requirements
		template<class T, std::enable_if_t<!std::is_pod_v<T>, int> = 0>
		void write(T const& val) {
			size_t const sizeBytes = val.size() * sizeof(T::value_type);
			for (size_t i = 0; i < sizeBytes; ++i) mData.emplace_back();
			memcpy(mData.data() + sizeBytes), val.data(), sizeBytes);
		}

		// Non template write function, takes data pointer and size in bytes
		inline void write(void const* data, size_t size) {
			for (size_t i = 0; i < size; ++i)
				mData.emplace_back(reinterpret_cast<std::byte const*>(data)[i]);
		}

		inline std::span<std::byte const> span() const { return mData; }
		inline operator std::span<std::byte const>() const { return mData; }

		std::vector<std::byte> mData;
	};
}