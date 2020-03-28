#include "chunk-tree.hpp"

#include <engine/core/memory.hpp>
#include <engine/math/math.hpp>

#include <cfloat>

#include "chunk.hpp"

ChunkTreeNode::ChunkTreeNode(int32 loadDistance)
		: ChunkTreeNode(Vector3f(-0.5f / Chunk::CHUNK_SIZE),
				Vector3f(loadDistance - 0.5f / Chunk::CHUNK_SIZE),
				Math::floorLog2(loadDistance), 0, nullptr) {}

ChunkTreeNode::ChunkTreeNode(const Vector3f& minExtents,
			const Vector3f& maxExtents, int32 maxLevel,
			int32 level, ChunkTreeNode* parent)
		: aabb(minExtents, maxExtents)
		, level(level)
		, maxLevel(maxLevel)
		, parent(parent)
		, empty(true) {

	if (level < maxLevel) {
		data = Memory::malloc(sizeof(ChildData));
		ChildData* cd = static_cast<ChildData*>(data);

		Memory::memset(cd->children, 0, 8 * sizeof(ChunkTreeNode*));

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

bool ChunkTreeNode::intersectsRay(const Vector3f& origin,
		const Vector3f& direction, Vector3i& intersectCoord,
		Vector3f& intersectPos, Vector3i& blockCoord, Vector3f& blockPos) const {
	float p1, p2;

	if (aabb.intersectsRay(origin, direction, p1, p2)) {
		if (level == maxLevel) {
			Chunk* chunk = reinterpret_cast<Chunk*>(data);

			if (chunk->isEmpty()) {
				return false;
			}

			if (chunk->getBlockTree().intersectsRay((origin - aabb.getMinExtents())
					* static_cast<float>(Chunk::CHUNK_SIZE) - 0.5f, direction,
					blockCoord, blockPos)) {
				intersectCoord = Vector3i(aabb.getCenter());
				intersectPos = Vector3f(intersectCoord)
						+ blockPos / static_cast<float>(Chunk::CHUNK_SIZE);

				return true;
			}

			return false;
		}

		Vector3i tempCoord;
		Vector3f tempPos;
		Vector3i tempBlockCoord;
		Vector3f tempBlockPos;

		ChildData* cd = static_cast<ChildData*>(data);

		float minDist = FLT_MAX;
		ChunkTreeNode* minChild = nullptr;

		for (auto* child : cd->children) {
			if (child && !child->empty && child->intersectsRay(origin,
					direction, tempCoord, tempPos, tempBlockCoord, tempBlockPos)) {
				const float len = Math::length(tempPos - origin);

				if (len < minDist) {
					minDist = len;
					minChild = child;

					intersectCoord = tempCoord;
					intersectPos = tempPos;
					blockCoord = tempBlockCoord;
					blockPos = tempBlockPos;
				}
			}
		}

		if (minChild) {
			return true;
		}
	}

	return false;
}

bool ChunkTreeNode::add(const Vector3i& position, Chunk* chunk) {
	if (!aabb.contains(position)) {
		return false;
	}
	
	if (level == maxLevel) {
		data = reinterpret_cast<void*>(chunk);
		empty = false;

		return true;
	}

	ChildData* cd = reinterpret_cast<ChildData*>(data);

	for (int32 i = 0; i < 8; ++i) {
		if (cd->childAABBs[i].contains(position)) {
			if (!cd->children[i]) {
				cd->children[i] = new ChunkTreeNode(cd->childAABBs[i].getMinExtents(),
						cd->childAABBs[i].getMaxExtents(), maxLevel, level + 1, this);
			}

			if (cd->children[i]->add(position, chunk)) {
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

bool ChunkTreeNode::remove(const Vector3i& position) {
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

		for (auto* child : cd->children) {
			if (child && !child->empty) {
				allEmpty = false;
				break;
			}
		}

		if (allEmpty) {
			empty = true;
		}
	}

	return removed;
}

void ChunkTreeNode::setAABB(const AABB& aabb) {
	this->aabb = aabb;
}

ChunkTreeNode** ChunkTreeNode::getChildren() {
	return reinterpret_cast<ChildData*>(data)->children;
}

Chunk* ChunkTreeNode::getChunk() {
	return reinterpret_cast<Chunk*>(data);
}

ChunkTreeNode::~ChunkTreeNode() {
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
