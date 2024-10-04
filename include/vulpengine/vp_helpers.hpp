#pragma once

#include "vp_transform.hpp"

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