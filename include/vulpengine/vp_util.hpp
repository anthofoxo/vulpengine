#pragma once

#include "vp_transform.hpp"

#include <vector>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <filesystem>

namespace vulpengine::helpers {
#ifdef VP_HAS_GLM
	// Handles moving a first person free move camera by modifying a Transform
	// 
	// `movement` is the sum of inputs that affect the direction,
	// this should be normalized, then multiplied with `deltaTime` and scaled by a factor to control movement speed
	// 
	// `rotation` controls rotating the camera, this will be the cursor delta position, this should be scaled for sensitivity
	//
	// Example Usage:
	// ```cpp
	// glm::vec3 movement{};
	// if (keys.contains(KEY_W)) --movement.z;
	// if (keys.contains(KEY_S)) ++movement.z;
	// if (keys.contains(KEY_A)) --movement.x;
	// if (keys.contains(KEY_D)) ++movement.x;
	// if (keys.contains(KEY_LEFT_SHIFT)) --movement.y;
	// if (keys.contains(KEY_SPACE)) ++movement.y;
	//
	// if (glm::length2(movement) > 0.0f) {
	//   movement = glm::normalize(movement) * deltaTime * moveSpeed;
	// }
	//
	// camera = vulpengine::helpers::freecam(camera, movement, cursorDelta * sensitivity);
	// ```
	Transform freecam(Transform const& transform, glm::vec3 const& movement, glm::vec2 const& rotation);
#endif
}

namespace vulpengine {
	struct StringMultiHash final {
		using hash_type = std::hash<std::string_view>;
		using is_transparent = void;
		std::size_t operator()(char const* str) const { return hash_type{}(str); }
		std::size_t operator()(std::string_view str) const { return hash_type{}(str); }
		std::size_t operator()(std::string const& str) const { return hash_type{}(str); }
	};

	template<class V> using UnorderedStringMap = std::unordered_map<std::string, V, StringMultiHash, std::equal_to<>>;

	std::optional<std::vector<char>> read_file(std::filesystem::path const& path);
}