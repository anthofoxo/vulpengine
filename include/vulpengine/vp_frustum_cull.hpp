#pragma once

#include "vp_features.hpp"

#ifdef VP_HAS_GLM

#include <glm/glm.hpp>

#include <array>

namespace vulpengine {
	class Frustum final {
	public:
		constexpr Frustum() noexcept = default;

		// viewProj = projection * view
		Frustum(glm::mat4 viewProj);
		bool intersect_aabb(glm::vec3 const& minp, glm::vec3 const& maxp) const;
	private:
		std::array<glm::vec4, 6> mPlanes{};
		std::array<glm::vec3, 8> mPoints{};
	};
}
#endif // VP_HAS_GLM