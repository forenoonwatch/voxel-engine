#pragma once

#include <engine/core/common.hpp>

#include <engine/math/aabb.hpp>

class BlockTreeNode {
	public:
		BlockTreeNode(int32 chunkSize);
		BlockTreeNode(const Vector3f& minExtents, const Vector3f& maxExtents,
				int32 maxLevel, int32 level, BlockTreeNode* parent);

		bool intersectsRay(const Vector3f& origin, const Vector3f& direction,
				Vector3i& intersectCoord, Vector3f& intersectPos) const;

		bool add(const Vector3i& position);
		bool remove(const Vector3i& position);

		void clear();

		inline const AABB& getAABB() const { return aabb; }

		inline BlockTreeNode* getParent() { return parent; }

		BlockTreeNode** getChildren();

		inline int32 getLevel() const { return level; }

		~BlockTreeNode();
	private:
		struct ChildData {
			BlockTreeNode* children[8];
			AABB childAABBs[8];
		};

		AABB aabb;

		int32 level;
		int32 maxLevel;

		BlockTreeNode* parent;
		void* data;

		bool empty;
};

