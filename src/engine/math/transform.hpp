#pragma once

#include <engine/math/vector.hpp>
#include <engine/math/matrix.hpp>
#include <engine/math/quaternion.hpp>

class Transform {
	public:
		FORCEINLINE Transform()
				: position(0.f, 0.f, 0.f)
				, rotation(1.f, 0.f, 0.f, 0.f) // wxyz
				, scale(1.f, 1.f, 1.f) {}

		FORCEINLINE Transform(const Vector3f& position)
				: position(position)
				, rotation(1.f, 0.f, 0.f, 0.f)
				, scale(1.f, 1.f, 1.f) {}

		FORCEINLINE Transform(const Quaternion& rotation)
				: position(0.f, 0.f, 0.f)
				, rotation(rotation)
				, scale(1.f, 1.f, 1.f) {}

		FORCEINLINE Transform(const Vector3f& position,
					const Quaternion& rotation, const Vector3f& scale)
				: position(position)
				, rotation(rotation)
				, scale(scale) {}

		FORCEINLINE Vector4f transform(const Vector4f& vector) const;
		FORCEINLINE Vector3f transform(const Vector3f& vector, float w) const;

		FORCEINLINE Vector4f inverseTransform(const Vector4f& vector) const;
		FORCEINLINE Vector3f inverseTransform(const Vector3f& vector,
				float w) const;

		FORCEINLINE Transform transform(const Transform& other) const;

		FORCEINLINE Matrix4f toMatrix() const;
		Matrix4f inverse() const;

		FORCEINLINE void normalizeRotation();
		FORCEINLINE bool isRotationNormalized();

		FORCEINLINE Transform operator+(const Transform& other) const;
		FORCEINLINE Transform operator+=(const Transform& other);
		FORCEINLINE Transform operator*(const Transform& other) const;
		FORCEINLINE Transform operator*=(const Transform& other);
		FORCEINLINE Transform operator*(float other) const;
		FORCEINLINE Transform operator*=(float other);

		FORCEINLINE Vector3f getPosition() const;
		FORCEINLINE Quaternion getRotation() const;
		FORCEINLINE Vector3f getScale() const;
		
		FORCEINLINE void set(const Vector3f& position,
				const Quaternion& rotation, const Vector3f& scale);
		FORCEINLINE void setPosition(const Vector3f& position);
		FORCEINLINE void setRotation(const Quaternion& rotation);
		FORCEINLINE void setScale(const Vector3f& scale);

		Transform& mix(const Transform& dest, float amt,
				Transform& result) const;
	private:
		Vector3f position;
		Quaternion rotation;
		Vector3f scale;
};

#include "engine/math/transform.inl"

