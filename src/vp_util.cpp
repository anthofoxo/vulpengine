#include "vulpengine/vp_util.hpp"

#include <fstream>

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

namespace vulpengine {
	std::optional<std::vector<char>> read_file(std::filesystem::path const& path) {
		std::ifstream file(path, std::ios::in | std::ios::binary);
		if (!file) return std::nullopt;

		std::vector<char> stream;
		file.seekg(0, std::ios::end);
		stream.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(stream.data(), stream.size());
		return stream;
	}
}