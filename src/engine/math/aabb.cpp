#include "engine/math/aabb.hpp"

#include "engine/core/memory.hpp"

#include "engine/math/math.hpp"

#include <cfloat>

AABB::AABB(const Vector3f* points, uint32 amt) {
	if (amt == 0) {
		extents[0] = Vector3f(0.f, 0.f, 0.f);
		extents[1] = Vector3f(0.f, 0.f, 0.f);
		
		return;
	}

	extents[0] = points[0];
	extents[1] = points[0];

	for (uint32 i = 1; i < amt; ++i) {
		extents[0] = Math::min(extents[0], points[i]);
		extents[1] = Math::max(extents[1], points[i]);
	}
}

AABB::AABB(float* points, uint32 amt, uint32 stride) {
	if (amt == 0) {
		extents[0] = Vector3f(0.f, 0.f, 0.f);
		extents[1] = Vector3f(0.f, 0.f, 0.f);

		return;
	}

	Vector3f initialPoint(points[0], points[1], points[2]);

	extents[0] = initialPoint;
	extents[1] = initialPoint;
	
	uintptr index = 3;
	stride += 3;
	
	for (uint32 i = 1; i < amt; ++i) {
		const Vector3f point(points[index], points[index + 1],
				points[index + 2]);

		extents[0] = Math::min(extents[0], point);
		extents[1] = Math::max(extents[1], point);

		index += stride;
	}
}

AABB AABB::transform(const Matrix4f& transform) const {
	const Vector4f vertices[] = {
		Vector4f(extents[0], 1.f),
		Vector4f(extents[1], 1.f),
		Vector4f(extents[1].x, extents[0].y, extents[0].z, 1.f),
		Vector4f(extents[0].x, extents[1].y, extents[0].z, 1.f),
		Vector4f(extents[0].x, extents[0].y, extents[1].z, 1.f),
		Vector4f(extents[0].x, extents[1].y, extents[1].z, 1.f),
		Vector4f(extents[1].x, extents[0].y, extents[1].z, 1.f),
		Vector4f(extents[1].x, extents[1].y, extents[0].z, 1.f)
	};

	Vector3f newMin(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3f newMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (uint32 i = 0; i < ARRAY_SIZE_IN_ELEMENTS(vertices); ++i) {
		const Vector3f p(transform * vertices[i]);

		newMin = Math::min(newMin, p);
		newMax = Math::max(newMax, p);
	}

	return AABB(newMin, newMax);
}

bool AABB::intersectsRay(const Vector3f& start, const Vector3f& rayDir,
		float& point1, float& point2) const {
	const Vector3f rdirVec = 1.f / rayDir;

	const Vector3f intersects1 = (extents[0] - start) * rdirVec;
	const Vector3f intersects2 = (extents[1] - start) * rdirVec;
	
	Vector3f mins = Math::min(intersects1, intersects2);
	Vector3f maxs = Math::max(intersects1, intersects2);

	if ((mins[0] > maxs[1]) || (mins[1] > maxs[0])) {
		return false;
	}

	if (mins[1] > mins[0]) {
		mins[0] = mins[1];
	}

	if (maxs[1] < maxs[0]) {
		maxs[0] = maxs[1];
	}

	if ((mins[0] > maxs[2]) || (mins[2] > maxs[0])) {
		return false;
	}

	if (mins[2] > mins[0]) {
		mins[0] = mins[2];
	}

	if (maxs[2] < maxs[0]) {
		maxs[0] = maxs[2];
	}
	
	point1 = mins[0];
	point2 = maxs[0];

	return true;
}

bool AABB::intersectsLine(const Vector3f& start, const Vector3f& end) const {
	const Vector3f dir = end - start;
	float p1, p2;
	
	bool intersect = intersectsRay(start, Math::normalize(dir), p1, p2);
	return intersect && p1 * p1 < Math::dot(dir, dir);
}

bool AABB::intersectsPlane(const Vector3f& position,
		const Vector3f& normal, float& r, float& s) const {
	const Vector3f c = (extents[1] + extents[0]) * 0.5f;
	const Vector3f e = extents[1] - c;

	r = Math::dot(e, glm::abs(normal)); // TODO: make better abs
	s = Math::dot(normal, c) + Math::dot(normal, position);
	// d = -dot(normal, position) for ax + by + cz + d = 0 form

	return s >= -r && s <= r;
}

bool AABB::belowPlane(const Vector3f& position,
		const Vector3f& normal, float& r, float& s) const {
	const Vector3f c = (extents[1] + extents[0]) * 0.5f;
	const Vector3f e = extents[1] - c;

	r = Math::dot(e, glm::abs(normal)); // TODO: make better abs
	s = Math::dot(normal, c - position);

	return s <= r;
}

