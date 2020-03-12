#pragma once

#include <engine/math/matrix.hpp>

#include <glm/gtc/quaternion.hpp>

#define Quaternion glm::quat

namespace Math {
	FORCEINLINE Quaternion conjugate(const Quaternion& q) {
		return glm::conjugate(q);
	}

	FORCEINLINE Quaternion rotate(const Quaternion& q, float angle,
			const Vector3f& axis) {
		return glm::rotate(q, angle, axis);
	}

	FORCEINLINE Quaternion rotateBy(const Quaternion& a, const Quaternion& b) {
		return glm::normalize(b * a);
	}

	FORCEINLINE Vector3f rotateBy(const Quaternion& q, const Vector3f& v) {
		// NOTE:
		// Active Rotation: q^-1 * v * q
		// Passive Rotation: q * v * q^-1
		// For rotation quats: conj(q) == q^-1
		
		Quaternion w = q * Quaternion(0.f, v.x, v.y, v.z) * glm::conjugate(q);
		return Vector3f(w.x, w.y, w.z);

		///Quaternion w = glm::conjugate(q) * v * q;
		//return Vector3f(w.x, w.y, w.z);
	}

	FORCEINLINE Quaternion normalize(const Quaternion& q) {
		return glm::normalize(q);
	}

	FORCEINLINE float length(const Quaternion& q) {
		return glm::length(q);
	}

	FORCEINLINE float dot(const Quaternion& a, const Quaternion& b) {
		return glm::dot(a, b);
	}

	FORCEINLINE Quaternion mix(const Quaternion& a, const Quaternion& b,
			float c) {
		return glm::mix(a, b, c);
	}

	FORCEINLINE Matrix3f quatToMat3(const Quaternion& q) {
		return glm::mat3_cast(q);
	}

	FORCEINLINE Quaternion mat3ToQuat(const Matrix3f& mat) {
		return glm::quat_cast(mat);
	}

	FORCEINLINE Matrix4f quatToMat4(const Quaternion& q) {
		return glm::mat4_cast(q);
	}

	FORCEINLINE Quaternion mat4ToQuat(const Matrix4f& mat) {
		return glm::quat_cast(mat);
	}
};

