#include "vulpengine/vp_frustum_cull.hpp"

#ifdef VP_HAS_GLM

namespace vulpengine {
	namespace {
		enum Planes {
			kLeft = 0,
			kRight,
			kBottom,
			kTop,
			kNear,
			kFar,
			kCount,
			kCombinations = kCount * (kCount - 1) / 2
		};

		template<Planes i, Planes j>
		struct ij2k final {
			enum { k = i * (9 - i) / 2 + j - 1 };
		};

		template<Planes a, Planes b, Planes c>
		glm::vec3 intersection(std::array<glm::vec4, 6> const& planes, std::array<glm::vec3, kCombinations> const& crosses) {
			float const d = glm::dot(glm::vec3(planes[a]), crosses[ij2k<b, c>::k]);
			glm::vec3 const res = glm::mat3(
				crosses[ij2k<b, c>::k], -crosses[ij2k<a, c>::k],
				crosses[ij2k<a, b>::k]) * glm::vec3(planes[a].w,
				planes[b].w, planes[c].w
			);
			return res * (-1.0f / d);
		}
	}

	Frustum::Frustum(glm::mat4 viewProj) {
		viewProj = glm::transpose(viewProj);
		mPlanes[kLeft]   = viewProj[3] + viewProj[0];
		mPlanes[kRight]  = viewProj[3] - viewProj[0];
		mPlanes[kBottom] = viewProj[3] + viewProj[1];
		mPlanes[kTop]    = viewProj[3] - viewProj[1];
		mPlanes[kNear]   = viewProj[3] + viewProj[2];
		mPlanes[kFar]    = viewProj[3] - viewProj[2];

		std::array<glm::vec3, kCombinations> crosses = {
			glm::cross(glm::vec3(mPlanes[kLeft]),   glm::vec3(mPlanes[kRight])),
			glm::cross(glm::vec3(mPlanes[kLeft]),   glm::vec3(mPlanes[kBottom])),
			glm::cross(glm::vec3(mPlanes[kLeft]),   glm::vec3(mPlanes[kTop])),
			glm::cross(glm::vec3(mPlanes[kLeft]),   glm::vec3(mPlanes[kNear])),
			glm::cross(glm::vec3(mPlanes[kLeft]),   glm::vec3(mPlanes[kFar])),
			glm::cross(glm::vec3(mPlanes[kRight]),  glm::vec3(mPlanes[kBottom])),
			glm::cross(glm::vec3(mPlanes[kRight]),  glm::vec3(mPlanes[kTop])),
			glm::cross(glm::vec3(mPlanes[kRight]),  glm::vec3(mPlanes[kNear])),
			glm::cross(glm::vec3(mPlanes[kRight]),  glm::vec3(mPlanes[kFar])),
			glm::cross(glm::vec3(mPlanes[kBottom]), glm::vec3(mPlanes[kTop])),
			glm::cross(glm::vec3(mPlanes[kBottom]), glm::vec3(mPlanes[kNear])),
			glm::cross(glm::vec3(mPlanes[kBottom]), glm::vec3(mPlanes[kFar])),
			glm::cross(glm::vec3(mPlanes[kTop]),    glm::vec3(mPlanes[kNear])),
			glm::cross(glm::vec3(mPlanes[kTop]),    glm::vec3(mPlanes[kFar])),
			glm::cross(glm::vec3(mPlanes[kNear]),   glm::vec3(mPlanes[kFar]))
		};

		mPoints[0] = intersection<kLeft,  kBottom, kNear>(mPlanes, crosses);
		mPoints[1] = intersection<kLeft,  kTop,    kNear>(mPlanes, crosses);
		mPoints[2] = intersection<kRight, kBottom, kNear>(mPlanes, crosses);
		mPoints[3] = intersection<kRight, kTop,    kNear>(mPlanes, crosses);
		mPoints[4] = intersection<kLeft,  kBottom, kFar >(mPlanes, crosses);
		mPoints[5] = intersection<kLeft,  kTop,    kFar >(mPlanes, crosses);
		mPoints[6] = intersection<kRight, kBottom, kFar >(mPlanes, crosses);
		mPoints[7] = intersection<kRight, kTop,    kFar >(mPlanes, crosses);

	}

	// http://iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
	bool Frustum::intersect_aabb(glm::vec3 const& minp, glm::vec3 const& maxp) const {
		// check box outside/inside of frustum
		for (int i = 0; i < kCount; i++) {
			if ((glm::dot(mPlanes[i], glm::vec4(minp.x, minp.y, minp.z, 1.0f)) < 0.0f) &&
				(glm::dot(mPlanes[i], glm::vec4(maxp.x, minp.y, minp.z, 1.0f)) < 0.0f) &&
				(glm::dot(mPlanes[i], glm::vec4(minp.x, maxp.y, minp.z, 1.0f)) < 0.0f) &&
				(glm::dot(mPlanes[i], glm::vec4(maxp.x, maxp.y, minp.z, 1.0f)) < 0.0f) &&
				(glm::dot(mPlanes[i], glm::vec4(minp.x, minp.y, maxp.z, 1.0f)) < 0.0f) &&
				(glm::dot(mPlanes[i], glm::vec4(maxp.x, minp.y, maxp.z, 1.0f)) < 0.0f) &&
				(glm::dot(mPlanes[i], glm::vec4(minp.x, maxp.y, maxp.z, 1.0f)) < 0.0f) &&
				(glm::dot(mPlanes[i], glm::vec4(maxp.x, maxp.y, maxp.z, 1.0f)) < 0.0f))
				return false;
		}

		// check frustum outside/inside box
		int out;
		out = 0; for (int i = 0; i < 8; ++i) out += ((mPoints[i].x > maxp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; ++i) out += ((mPoints[i].x < minp.x) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; ++i) out += ((mPoints[i].y > maxp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; ++i) out += ((mPoints[i].y < minp.y) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; ++i) out += ((mPoints[i].z > maxp.z) ? 1 : 0); if (out == 8) return false;
		out = 0; for (int i = 0; i < 8; ++i) out += ((mPoints[i].z < minp.z) ? 1 : 0); if (out == 8) return false;

		return true;
	}
}
#endif // VP_HAS_GLM