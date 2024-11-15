#include "vulpengine/vp_transform.hpp"

#ifdef VP_HAS_GLM

#include <glm/gtx/matrix_decompose.hpp>

namespace vulpengine {
	glm::mat4 Transform::get() const {
		glm::vec3 skew = glm::vec3(0.0f);
		glm::vec4 perspective(0.0f, 0.0f, 0.0f, 1.0f);
		return glm::recompose(scale, orientation, position, skew, perspective);
	}

	void Transform::set(glm::mat4 const& mat) {
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(mat, scale, orientation, position, skew, perspective);
	}

	void Transform::translate(glm::vec3 const& direction) {
		glm::mat4 const result = glm::translate(get(), direction);
		position = result[3]; // Only care about position
	}
}
#endif // VP_HAS_GLM