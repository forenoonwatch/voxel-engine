#pragma once

#include <engine/core/common.hpp>

#include <engine/math/vector.hpp>

namespace Intersects {
	static FORCEINLINE bool intersectTriangle(const Vector3f& pos,
			const Vector3f& dir, const Vector3f& v0, const Vector3f& v1,
			const Vector3f& v2, Vector3f& intersectPos, Vector3f& normal) {
		const Vector3f e1 = v1 - v0;
		const Vector3f e2 = v2 - v0;
		
		const Vector3f h = Math::cross(dir, e2);
		const float a = Math::dot(e1, h);

		if (a > -0.00001f && a < 0.00001f) {
			return false;
		}

		float f = 1.f / a;

		Vector3f s = pos - v0;
		float u = f * Math::dot(s, h);

		if (u < 0.f || u > 1.f) {
			return false;
		}

		Vector3f q = Math::cross(s, e1);
		float v = f * Math::dot(dir, q);

		if (v < 0.f || u + v > 1.f) {
			return false;
		}

		f *= Math::dot(e2, q);

		if (f > 0.00001f) {
			intersectPos = pos + dir * f;
			normal = Math::normalize(Math::cross(e1, e2));

			return true;
		}

		return false;
	}

	static FORCEINLINE bool intersectTriangle(const Vector3f& pos,
			const Vector3f& dir, const Vector3f& v0, const Vector3f& v1,
			const Vector3f& v2, float& f) {
		const Vector3f e1 = v1 - v0;
		const Vector3f e2 = v2 - v0;
		
		const Vector3f h = Math::cross(dir, e2);
		const float a = Math::dot(e1, h);

		if (a > -0.00001f && a < 0.00001f) {
			return false;
		}

		f = 1.f / a;

		Vector3f s = pos - v0;
		float u = f * Math::dot(s, h);

		if (u < 0.f || u > 1.f) {
			return false;
		}

		Vector3f q = Math::cross(s, e1);
		float v = f * Math::dot(dir, q);

		if (v < 0.f || u + v > 1.f) {
			return false;
		}

		f *= Math::dot(e2, q);

		if (f > 0.00001f) {
			return true;
		}

		return false;
	}
};

