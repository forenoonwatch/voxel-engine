#pragma once

#include <engine/core/string.hpp>

#include <engine/rendering/indexed-model.hpp>

// TODO: StringView
namespace AssetLoader {
	bool loadAssets(const String& fileName,
			const IndexedModel::AllocationHints& staticAllocHints,
			ArrayList<IndexedModel>& models);
};
