#include "block-tree.hpp"

#include <engine/core/memory.hpp>
#include <engine/math/math.hpp>

#include <cfloat>

BlockTreeNode::BlockTreeNode(int32 chunkSize)
		: BlockTreeNode(Vector3f(-0.5f), Vector3f(chunkSize - 0.5f),
				Math::floorLog2(chunkSize), 0, nullptr) {}

BlockTreeNode::BlockTreeNode(const Vector3f& minExtents,
			const Vector3f& maxExtents, int32 maxLevel,
			int32 level, BlockTreeNode* parent)
		: aabb(minExtents, maxExtents)
		, level(level)
		, maxLevel(maxLevel)
		, parent(parent)
		, empty(true) {
	if (level < maxLevel) {
		data = Memory::malloc(sizeof(ChildData));
		ChildData* cd = static_cast<ChildData*>(data);

		Memory::memset(cd->children, 0, 8 * sizeof(BlockTreeNode*));

		const Vector3f center = aabb.getCenter();
		const Vector3f extents = aabb.getExtents();

		int32 i = 0;

		for (float z = -1.f; z <= 1.f; z += 2.f) {
			for (float y = -1.f; y <= 1.f; y += 2.f) {
				for (float x = -1.f; x <= 1.f; x += 2.f) {
					const Vector3f corner = center + extents * Vector3f(x, y, z);

					cd->childAABBs[i++] = AABB(Math::min(center, corner),
							Math::max(center, corner));
				}
			}
		}
	}
	else {
		data = nullptr;
	}
}

bool BlockTreeNode::intersectsRay(const Vector3f& origin,
		const Vector3f& direction, Vector3i& intersectCoord,
		Vector3f& intersectPos) const {
	float p1, p2;

	if (aabb.intersectsRay(origin, direction, p1, p2)) {
		if (level == maxLevel) {
			if (p1 > 0.f) {
				intersectCoord = static_cast<Vector3i>(aabb.getCenter());
				intersectPos = origin + direction * p1;

				return true;
			}
			else {
				return false;
			}
		}

		ChildData* cd = static_cast<ChildData*>(data);

		Vector3i tempCoord;
		Vector3f tempPos;

		float minDist = FLT_MAX;
		BlockTreeNode* minChild = nullptr;

		for (auto* child : cd->children) {
			if (child && !child->empty && child->intersectsRay(origin,
					direction, tempCoord, tempPos)) {
				const float len = Math::length(tempPos - origin);

				if (len < minDist) {
					minDist = len;
					minChild = child;

					intersectCoord = tempCoord;
					intersectPos = tempPos;
				}
			}
		}

		if (minChild) {
			return true;
		}
	}

	return false;
}

bool BlockTreeNode::add(const Vector3i& position) {
	if (!aabb.contains(position)) {
		return false;
	}
	
	if (level == maxLevel) {
		empty = false;

		return true;
	}

	ChildData* cd = reinterpret_cast<ChildData*>(data);

	for (int32 i = 0; i < 8; ++i) {
		if (cd->childAABBs[i].contains(position)) {
			if (!cd->children[i]) {
				cd->children[i] = new BlockTreeNode(cd->childAABBs[i].getMinExtents(),
						cd->childAABBs[i].getMaxExtents(), maxLevel, level + 1, this);
			}

			if (cd->children[i]->add(position)) {
				empty = false;

				return true;
			}
			else {
				return false;
			}
		}
	}

	return false;
}

bool BlockTreeNode::remove(const Vector3i& position) {
	if (empty) {
		return false;
	}

	if (level == maxLevel) {
		empty = true;
		return true;
	}

	ChildData* cd = reinterpret_cast<ChildData*>(data);

	bool removed = false;

	for (int32 i = 0; i < 8; ++i) {
		if (cd->children[i] && cd->childAABBs[i].contains(position)) {
			removed = cd->children[i]->remove(position);
			break;
		}
	}

	if (removed) {
		bool allEmpty = true;

		for (int32 i = 0; i < 8; ++i) {
			if (cd->children[i]) {
				if (cd->children[i]->empty) {
					delete cd->children[i];
					cd->children[i] = nullptr;
				}
				else {
					allEmpty = false;
				}
			}
		}

		if (allEmpty) {
			empty = true;
		}
	}

	return removed;
}

void BlockTreeNode::clear() {
	ChildData* cd = reinterpret_cast<ChildData*>(data);

	for (int32 i = 0; i < 8; ++i) {
		if (cd->children[i]) {
			delete cd->children[i];
			cd->children[i] = nullptr;
		}
	}
}

BlockTreeNode** BlockTreeNode::getChildren() {
	return reinterpret_cast<ChildData*>(data)->children;
}

BlockTreeNode::~BlockTreeNode() {
	if (level < maxLevel) {
		ChildData* cd = static_cast<ChildData*>(data);

		for (int32 i = 0; i < 8; ++i) {
			if (cd->children[i]) {
				delete cd->children[i];
			}
		}

		Memory::free(data);
	}
}
