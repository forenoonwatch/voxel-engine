#pragma once

#include <engine/math/vector.hpp>

#include <glm/gtc/matrix_transform.hpp>

#define Matrix3f glm::mat3
#define Matrix4f glm::mat4

namespace Math {
	template <typename T>
	FORCEINLINE T inverse(const T& matrix) {
		return glm::inverse(matrix);
	}

	template <typename T>
	FORCEINLINE T transpose(const T& matrix) {
		return glm::transpose(matrix);
	}

	FORCEINLINE Matrix3f outerProduct(const Vector3f& a, const Vector3f& b) {
		return glm::outerProduct(a, b);
	}

	FORCEINLINE Matrix4f translate(const Matrix4f& matrix,
			const Vector3f& translation) {
		return glm::translate(matrix, translation);
	}

	FORCEINLINE Matrix4f rotate(const Matrix4f& matrix, float angle,
			const Vector3f& axis) {
		return glm::rotate(matrix, angle, axis);
	}

	FORCEINLINE Matrix4f scale(const Matrix4f& matrix, const Vector3f& size) {
		return glm::scale(matrix, size);
	}

	FORCEINLINE Matrix4f perspective(float fieldOfView, float aspectRatio,
			float zNear, float zFar) {
		return glm::perspective(fieldOfView, aspectRatio, zNear, zFar);
	}
};
