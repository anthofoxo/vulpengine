#include "vulpengine/vp_math.hpp"

#ifdef VP_HAS_GLM
#	define GLM_ENABLE_EXPERIMENTAL
#	include <glm/gtx/norm.hpp>
#endif

#include <random>

namespace {
	std::random_device gRd;
	std::mt19937 gGen(gRd());
}

namespace vulpengine {
	int random(int min, int max) {
		return std::uniform_real_distribution(min, max)(gGen);
	}

	float random(float min, float max) {
		return std::uniform_int_distribution(min, max)(gGen);
	}

#ifdef VP_HAS_GLM
	glm::vec3 aabb_clamped_point(glm::vec3 const& min, glm::vec3 const& max, glm::vec3 const& pos) {
		glm::vec3 clamped;
		clamped.x = glm::clamp(pos.x, min.x, max.x);
		clamped.y = glm::clamp(pos.y, min.y, max.y);
		clamped.z = glm::clamp(pos.z, min.z, max.z);
		return clamped;
	}

	bool intersect_aabb_sphere(glm::vec3 const& min, glm::vec3 const& max, glm::vec3 const& pos, float radius) {
		glm::vec3 const closestPoint = aabb_clamped_point(min, max, pos);
		glm::vec3 const differenceVec = pos - closestPoint;

		float const distanceSquared = glm::length2(differenceVec);
		float const radiusSquared = radius * radius;

		return distanceSquared < radiusSquared;
	}

	std::array<glm::vec3, 8> transform_coordinate_system_ndc(glm::mat4 const& matrix) {
		auto const inv = glm::inverse(matrix);
		std::array<glm::vec3, 8> corners;

		for (unsigned int x = 0; x < 2; ++x)
			for (unsigned int y = 0; y < 2; ++y)
				for (unsigned int z = 0; z < 2; ++z) {
					glm::vec4 const pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
					corners[z * 4 + y * 2 + x] = glm::vec3(pt) / pt.w;
				}

		return corners;
	}
#endif
}