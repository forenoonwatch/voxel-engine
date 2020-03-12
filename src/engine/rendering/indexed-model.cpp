#include "engine/rendering/indexed-model.hpp"

#include "engine/math/intersects.hpp"

#include <cfloat>

#define EPSILON 1e-6f

static float tetrahedronVolume(Vector3f& centroidSum, const Vector3f& p,
		const Vector3f& v1, const Vector3f& v2, const Vector3f& v3);

static float clipTriangle(Vector3f& centroidSum, const Vector3f& p,
		const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, float d1,
		float d2, float d3);

IndexedModel::IndexedModel(const AllocationHints& hints)
		: instancedElementStartIndex(hints.instancedElementStartIndex)
		, flags(hints.flags) {
	elementSizes.assign(hints.elementSizes.begin(), hints.elementSizes.end());
	elements.resize(hints.elementSizes.size());
}

bool IndexedModel::intersectsRay(const Vector3f& pos, const Vector3f& dir,
		Vector3f& intersectPos, Vector3f& normal) const {
	uint32 i0, i1, i2;
	uint32 minI0, minI1, minI2;

	float t;
	float minT = FLT_MAX;
	bool intersected = false;
	
	for (uint32 i = 0; i < indices.size(); i += 3) {
		i0 = indices[i];
		i1 = indices[i + 1];
		i2 = indices[i + 2];

		/*Vector3f v0 = Vector3f(&elements[0][3 * i0]);
		Vector3f v1 = Vector3f(&elements[0][3 * i1]);
		Vector3f v2 = Vector3f(&elements[0][3 * i2]);*/

		const Vector3f& v0 = *((Vector3f*)&elements[0][3 * i0]);
		const Vector3f& v1 = *((Vector3f*)&elements[0][3 * i1]);
		const Vector3f& v2 = *((Vector3f*)&elements[0][3 * i2]);

		if (Intersects::intersectTriangle(pos, dir, v0, v1, v2, t)
				&& t < minT) {
			minT = t;
			intersected = true;

			minI0 = i0;
			minI1 = i1;
			minI2 = i2;
		}
	}

	if (intersected) {
		intersectPos = pos + dir * minT;

		const Vector3f& n0 = *((Vector3f*)&elements[2][3 * minI0]);
		const Vector3f& n1 = *((Vector3f*)&elements[2][3 * minI1]);
		const Vector3f& n2 = *((Vector3f*)&elements[2][3 * minI2]);

		normal = (n0 + n1 + n2) / 3.f;
	}

	return intersected;
}

float IndexedModel::calcSubmergedVolume(const Vector3f& planePosition,
		const Vector3f& planeNormal, Vector3f& centroidSum) const {
	float* ds = new float[getNumVertices()];

	uint32 numSubmerged = 0;
	uint32 sampleVert = 0;

	for (uint32 i = 0; i < getNumVertices(); ++i) {
		const Vector3f& vert = *((Vector3f*)&elements[0][3 * i]);

		ds[i] = Math::dot(planeNormal, vert - planePosition);

		if (ds[i] < -EPSILON) {
			++numSubmerged;
			sampleVert = i;
		}
	}

	if (numSubmerged == 0) {
		centroidSum = Vector3f(0.f, 0.f, 0.f);
		delete[] ds;

		return 0;
	}

	Vector3f p = *((Vector3f*)&elements[0][3 * sampleVert])
			- ds[sampleVert] * planeNormal;

	float volume = 0;
	centroidSum = Vector3f(0.f, 0.f, 0.f);

	for (uint32 i = 0; i < indices.size(); i += 3) {
		uint32 i1 = indices[i];
		uint32 i2 = indices[i + 1];
		uint32 i3 = indices[i + 2];

		const Vector3f& v1 = *((Vector3f*)&elements[0][3 * i1]);
		float d1 = ds[i1];

		const Vector3f& v2 = *((Vector3f*)&elements[0][3 * i2]);
		float d2 = ds[i2];

		const Vector3f& v3 = *((Vector3f*)&elements[0][3 * i3]);
		float d3 = ds[i3];

		if (d1 * d2 < 0) {
			volume += clipTriangle(centroidSum, p, v1, v2, v3, d1, d2, d3);
		}
		else if (d1 * d3 < 0) {
			volume += clipTriangle(centroidSum, p, v3, v1, v2, d3, d1, d2);
		}
		else if (d2 * d3 < 0) {
			volume += clipTriangle(centroidSum, p, v2, v3, v1, d2, d3, d1);
		}
		else if (d1 < 0 || d2 < 0 || d3 < 0) {
			volume += tetrahedronVolume(centroidSum, p, v1, v2, v3);
		}
	}

	if (volume <= EPSILON) {
		centroidSum = Vector3f(0.f, 0.f, 0.f);
		delete[] ds;

		return 0;
	}

	//centroidSum /= volume;

	delete[] ds;
	return volume;
}

void IndexedModel::initStaticMesh() {
	allocateElement(3); // Positions
	allocateElement(2); // TexCoords
	allocateElement(3); // Normals
	allocateElement(3); // Tangents
	allocateElement(2 * 16); // MVP, model matrix

	setInstancedElementStartIndex(4); // Begin instanced data
}

void IndexedModel::addElement1f(uint32 elementIndex, float e0) {
	elements[elementIndex].push_back(e0);
}

void IndexedModel::addElement2f(uint32 elementIndex, float e0, float e1) {
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
}

void IndexedModel::addElement3f(uint32 elementIndex, float e0, float e1,
		float e2) {
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
	elements[elementIndex].push_back(e2);
}

void IndexedModel::addElement4f(uint32 elementIndex, float e0, float e1,
		float e2, float e3) {
	elements[elementIndex].push_back(e0);
	elements[elementIndex].push_back(e1);
	elements[elementIndex].push_back(e2);
	elements[elementIndex].push_back(e3);
}

void IndexedModel::setElement4f(uint32 elementIndex, uint32 arrayIndex,
		float e0, float e1, float e2, float e3) {
	arrayIndex *= elementSizes[elementIndex];

	elements[elementIndex][arrayIndex] = e0;
	elements[elementIndex][arrayIndex + 1] = e1;
	elements[elementIndex][arrayIndex + 2] = e2;
	elements[elementIndex][arrayIndex + 3] = e3;
}

void IndexedModel::addIndices1i(uint32 i0) {
	indices.push_back(i0);
}

void IndexedModel::addIndices2i(uint32 i0, uint32 i1) {
	indices.push_back(i0);
	indices.push_back(i1);
}

void IndexedModel::addIndices3i(uint32 i0, uint32 i1, uint32 i2) {
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
}

void IndexedModel::addIndices4i(uint32 i0, uint32 i1, uint32 i2, uint32 i3) {
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i3);
}

static float tetrahedronVolume(Vector3f& centroidSum, const Vector3f& p,
		const Vector3f& v1, const Vector3f& v2, const Vector3f& v3) {
	Vector3f a = v2 - v1;
	Vector3f b = v3 - v1;
	Vector3f r = p - v1;

	const float volume = Math::dot(Math::cross(b, a) * (1.f / 6.f), r);
	
	centroidSum += 0.25f * volume*(v1 + v2 + v3 + p);
	return volume;
}

static float clipTriangle(Vector3f& centroidSum, const Vector3f& p,
		const Vector3f& v1, const Vector3f& v2, const Vector3f& v3, float d1,
		float d2, float d3) {
	Vector3f vc1 = v1 + (d1 / (d1 - d2)) * (v2 - v1);
	float volume = 0;

	if (d1 < 0) {
		if (d3 < 0) {
			Vector3f vc2 = v2 + (d2 / (d2 - d3)) * (v3 - v2);
			volume += tetrahedronVolume(centroidSum, p, vc1, vc2, v1);
			volume += tetrahedronVolume(centroidSum, p, vc2, v3, v1);
		}
		else {
			Vector3f vc2 = v1 + (d1 / (d1 - d3)) * (v3 - v1);
			volume += tetrahedronVolume(centroidSum, p, vc1, vc2, v1);
		}
	}
	else {
		if (d3 < 0) {
			Vector3f vc2 = v1 + (d1 / (d1 - d3)) * (v3 - v1);
			volume += tetrahedronVolume(centroidSum, p, vc1, v2, v3);
			volume += tetrahedronVolume(centroidSum, p, vc1, v3, vc2);
		}
		else {
			Vector3f vc2 = v2 + (d2 / (d2 - d3)) * (v3 - v2);
			volume += tetrahedronVolume(centroidSum, p, vc1, v2, vc2);
		}
	}

	return volume;
}

