#include "engine/math/transform.hpp"

Matrix4f Transform::inverse() const {
	return Math::inverse(toMatrix());
}

Transform& Transform::mix(const Transform& dest, float amt,
		Transform& result) const {
	result.setPosition(Math::mix(position, dest.getPosition(), amt));
	result.setRotation(Math::mix(rotation, dest.getRotation(), amt));
	result.setScale(Math::mix(scale, dest.getScale(), amt));

	return result;
}

