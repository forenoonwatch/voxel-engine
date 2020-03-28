#pragma once

#include <engine/core/common.hpp>

#include <engine/math/aabb.hpp>

class Chunk;

class ChunkTreeNode {
	public:
		ChunkTreeNode(int32 loadDistance);
		ChunkTreeNode(const Vector3f& minExtents, const Vector3f& maxExtents,
				int32 maxLevel, int32 level, ChunkTreeNode* parent);

		bool intersectsRay(const Vector3f& origin, const Vector3f& direction,
				Vector3i& chunkCoord, Vector3f& chunkPos, Vector3i& blockCoord,
				Vector3f& blockPos) const;

		bool add(const Vector3i& position, Chunk* chunk);
		bool remove(const Vector3i& position);

		void setAABB(const AABB& aabb);

		inline const AABB& getAABB() const { return aabb; }

		inline ChunkTreeNode* getParent() { return parent; }

		ChunkTreeNode** getChildren();
		Chunk* getChunk();

		inline int32 getLevel() const { return level; }

		~ChunkTreeNode();
	private:
		struct ChildData {
			ChunkTreeNode* children[8];
			AABB childAABBs[8];
		};

		AABB aabb;

		int32 level;
		int32 maxLevel;

		ChunkTreeNode* parent;
		void* data;

		bool empty;
};

