#pragma once

/*!
Simple set of math functions
*/

#include "vulpengine/vp_features.hpp"

#ifdef VP_HAS_GLM
#	include <glm/glm.hpp>
#endif

#include <array>

namespace vulpengine::math {
	template<class T>
	[[nodiscard]] T map(T value, T min1, T max1, T min2, T max2) {
		return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
	}

#ifdef VP_HAS_GLM
	[[nodiscard]] glm::vec3 aabb_clamped_point(glm::vec3 const& min, glm::vec3 const& max, glm::vec3 const& pos);
	[[nodiscard]] bool intersect_aabb_sphere(glm::vec3 const& min, glm::vec3 const& max, glm::vec3 const& pos, float radius);
	[[nodiscard]] std::array<glm::vec3, 8> transform_coordinate_system_ndc(glm::mat4 const& matrix);
#endif
}