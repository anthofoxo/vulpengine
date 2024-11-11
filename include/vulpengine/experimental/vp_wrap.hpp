#pragma once

/*!
A simple wrapper type to hold onto reference types, useful for creating buffers, vertex arrays, and meshes

Needs Improvment:
- Needs more testing, will likely rename to Box
*/

#include <type_traits>
#include <utility>

namespace vulpengine::experimental {
	template<class T> struct Wrap { T value; };

	template<class T>
	Wrap<std::remove_reference_t<T>&&> wrap_rvref(T&& value) {
		return { std::move(value) };
	}

	template<class T>
	Wrap<std::remove_reference_t<T> const&> wrap_cref(T&& value) {
		return { value };
	}
}