#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace vulpengine {
	struct StringMultiHash final {
		using hash_type = std::hash<std::string_view>;
		using is_transparent = void;
		std::size_t operator()(char const* str) const { return hash_type{}(str); }
		std::size_t operator()(std::string_view str) const { return hash_type{}(str); }
		std::size_t operator()(std::string const& str) const { return hash_type{}(str); }
	};

	template<class V> using UnorderedStringMap = std::unordered_map<std::string, V, StringMultiHash, std::equal_to<>>;
}