
FORCEINLINE Matrix4f Transform::toMatrix() const {
	return Math::translate(Matrix4f(1.f), position)
			* Math::quatToMat4(rotation)
			* Math::scale(Matrix4f(1.f), scale);
}

FORCEINLINE void Transform::normalizeRotation() {
	rotation = Math::normalize(rotation);
}

FORCEINLINE bool Transform::isRotationNormalized() {
	const float d = Math::dot(rotation, rotation);
	return d >= 0.999f && d <= 1.0001f;
}

FORCEINLINE Transform Transform::operator+(const Transform& other) const {
	return Transform(position + other.position,
			rotation + other.rotation, scale + other.scale);
}

FORCEINLINE Transform Transform::operator+=(const Transform& other) {
	position += other.position;
	rotation += other.rotation;
	scale += other.scale;

	return *this;
}

FORCEINLINE Transform Transform::operator*(const Transform& other) const {
	return Transform(position * other.position,
			rotation * other.rotation, scale * other.scale);
}

FORCEINLINE Transform Transform::operator*=(const Transform& other) {
	position *= other.position;
	rotation *= other.rotation;
	scale *= other.scale;

	return *this;
}

FORCEINLINE Transform Transform::operator*(float other) const {
	return Transform(position * other,
			rotation * other, scale * other);
}

FORCEINLINE Transform Transform::operator*=(float other) {
	position *= other;
	rotation *= other;
	scale *= other;
	return *this;
}

FORCEINLINE Vector3f Transform::transform(const Vector3f& vector,
		float w) const {
	return Vector3f(toMatrix() * Vector4f(vector, w));
}

FORCEINLINE Vector4f Transform::transform(const Vector4f& vector) const {
	return toMatrix() * vector;
}

FORCEINLINE Vector3f Transform::inverseTransform(const Vector3f& vector,
		float w) const {
	return Vector3f(inverse() * Vector4f(vector, w));
}

FORCEINLINE Vector4f Transform::inverseTransform(
		const Vector4f& vector) const {
	return inverse() * vector;
}

FORCEINLINE Transform Transform::transform(const Transform& other) const {
	return Transform(position + Math::rotateBy(rotation, other.position),
			Math::rotateBy(other.rotation, rotation), scale * other.scale);
}

FORCEINLINE Vector3f Transform::getPosition() const {
	return position;
}

FORCEINLINE Quaternion Transform::getRotation() const {
	return rotation;
}

FORCEINLINE Vector3f Transform::getScale() const {
	return scale;
}

FORCEINLINE void Transform::set(const Vector3f& position,
		const Quaternion& rotation, const Vector3f& scale) {
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
}

FORCEINLINE void Transform::setPosition(const Vector3f& val) {
	position = val;
}

FORCEINLINE void Transform::setRotation(const Quaternion& val) {
	rotation = val;
}

FORCEINLINE void Transform::setScale(const Vector3f& val) {
	scale = val;
}

