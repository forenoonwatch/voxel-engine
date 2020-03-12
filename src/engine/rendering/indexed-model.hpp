#pragma once

#include <engine/core/common.hpp>
#include <engine/core/array-list.hpp>

#include <engine/math/vector.hpp>

class IndexedModel {
	public:
		enum AllocationFlags {
			FLAG_INTERLEAVED_INSTANCES = 0x1,
		};

		struct AllocationHints {
			ArrayList<uint32> elementSizes;
			uint32 instancedElementStartIndex = (uint32)-1;
			uint32 flags = 0;
		};

		inline IndexedModel()
				: instancedElementStartIndex((uint32)-1)
				, flags(0) {}

		IndexedModel(const AllocationHints& hints);

		bool intersectsRay(const Vector3f& pos, const Vector3f& dir,
				Vector3f& intersectPos, Vector3f& normal) const;

		float calcSubmergedVolume(const Vector3f& planePosition,
				const Vector3f& planeNormal, Vector3f& centroidSum) const;

		void initStaticMesh();

		inline void allocateElement(uint32 elementSize);
		inline void setInstancedElementStartIndex(uint32 elementIndex);

		void addElement1f(uint32 elementIndex, float e0);
		void addElement2f(uint32 elementIndex, float e0, float e1);
		void addElement3f(uint32 elementIndex, float e0, float e1, float e2);
		void addElement4f(uint32 elementIndex, float e0, float e1, float e2,
				float e3);

		void setElement4f(uint32 elementIndex, uint32 arrayIndex,
				float e0, float e1, float e2, float e3);

		void addIndices1i(uint32 i0);
		void addIndices2i(uint32 i0, uint32 i1);
		void addIndices3i(uint32 i0, uint32 i1, uint32 i2);
		void addIndices4i(uint32 i0, uint32 i1, uint32 i2, uint32 i3);

		inline ArrayList<const float*> getVertexData() const;
		inline const uint32* getIndices() const;
		inline const uint32* getElementSizes() const;
		inline uint32 getElementArraySize(uint32 elementIndex) const;

		inline uint32 getNumVertexComponents() const;
		inline uint32 getNumInstanceComponents() const;

		inline uint32 getNumVertices() const;
		inline uint32 getNumIndices() const;
		inline uint32 getInstancedElementStartIndex() const;
		inline uint32 getFlags() const;

		inline float getElement1f(uint32 elementIndex,
				uint32 arrayIndex) const;
		inline Vector2f getElement2f(uint32 elementIndex,
				uint32 arrayIndex) const;
		inline Vector3f getElement3f(uint32 elementIndex,
				uint32 arrayIndex) const;
		inline Vector4f getElement4f(uint32 elementIndex,
				uint32 arrayIndex) const;
	private:
		ArrayList<uint32> indices;
		ArrayList<uint32> elementSizes;
		ArrayList<ArrayList<float>> elements;

		uint32 instancedElementStartIndex;
		uint32 flags;
};

#include "indexed-model.inl"

