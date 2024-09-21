#pragma once

#include "vp_features.hpp"

#ifdef VP_HAS_GLM

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace vulpengine {
	struct Transform final {
		glm::vec3 position{};
		glm::quat orientation = glm::identity<glm::quat>();
		glm::vec3 scale = glm::vec3(1.0f);

		glm::mat4 get() const;
		void set(glm::mat4 const& mat);
		void translate(glm::vec3 const& direction);
	};
}
#endif // VP_HAS_GLM