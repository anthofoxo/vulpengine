#include "vp_helpers.hpp"

#ifdef VP_HAS_GLM
#	include <glm/gtx/norm.hpp>
#endif

namespace vulpengine::helpers {
#ifdef VP_HAS_GLM
	Transform freecam(Transform const& transform, glm::vec3 const& movement, glm::vec2 const& rotation) {
		Transform out = transform;

		if (glm::length2(movement) > 0) {
			out.translate(movement);
		}

		if (glm::length2(rotation) > 0) {
			glm::vec3 const up = glm::vec3(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f) * out.get());
			out.orientation = glm::rotate(out.orientation, glm::radians(-rotation.x), up);
			out.orientation = glm::rotate(out.orientation, glm::radians(-rotation.y), { 1.0f, 0.0f, 0.0f });
		}

		return out;
	}
#endif
}