
inline void IndexedModel::allocateElement(uint32 elementSize) {
	elementSizes.push_back(elementSize);
	elements.emplace_back();
}

inline void IndexedModel::setInstancedElementStartIndex(uint32 elementIndex) {
	instancedElementStartIndex = elementIndex;
}

inline ArrayList<const float*> IndexedModel::getVertexData() const {
	ArrayList<const float*> vertexData;

	for (uint32 i = 0; i < getNumVertexComponents(); ++i) {
		vertexData.push_back(&elements[i][0]);
	}

	return vertexData;
}

inline const uint32* IndexedModel::getIndices() const {
	return &indices[0];
}

inline const uint32* IndexedModel::getElementSizes() const {
	return &elementSizes[0];
}

inline uint32 IndexedModel::getElementArraySize(uint32 elementIndex) const {
	return elements[elementIndex].size();
}

inline uint32 IndexedModel::getNumVertexComponents() const {
	return elementSizes.size() - getNumInstanceComponents();
}

inline uint32 IndexedModel::getNumInstanceComponents() const {
	return instancedElementStartIndex == ((uint32)-1)
		? 0 : (elementSizes.size() - instancedElementStartIndex);
}

inline uint32 IndexedModel::getNumVertices() const {
	return elements[0].size() / elementSizes[0];
}

inline uint32 IndexedModel::getNumIndices() const {
	return indices.size();
}

inline uint32 IndexedModel::getInstancedElementStartIndex() const {
	return instancedElementStartIndex;
}

inline uint32 IndexedModel::getFlags() const {
	return flags;
}

inline float IndexedModel::getElement1f(uint32 elementIndex,
		uint32 arrayIndex) const {
	return elements[elementIndex][arrayIndex];
}

inline Vector2f IndexedModel::getElement2f(uint32 elementIndex,
		uint32 arrayIndex) const {
	return Vector2f(elements[elementIndex][arrayIndex],
			elements[elementIndex][arrayIndex + 1]);
}

inline Vector3f IndexedModel::getElement3f(uint32 elementIndex,
		uint32 arrayIndex) const {
	return Vector3f(elements[elementIndex][arrayIndex],
			elements[elementIndex][arrayIndex + 1],
			elements[elementIndex][arrayIndex + 2]);
}

inline Vector4f IndexedModel::getElement4f(uint32 elementIndex,
		uint32 arrayIndex) const {
	return Vector4f(elements[elementIndex][arrayIndex],
			elements[elementIndex][arrayIndex + 1],
			elements[elementIndex][arrayIndex + 2],
			elements[elementIndex][arrayIndex + 3]);
}

