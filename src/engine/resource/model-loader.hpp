#pragma once

#include <engine/game/resource-loader.hpp>

#include <engine/rendering/indexed-model.hpp>

#include <engine/core/asset-loader.hpp>

class ModelLoader final
		: public ResourceLoader<ModelLoader, IndexedModel> {
	public:
		Memory::SharedPointer<IndexedModel> load(const char* fileName,
				const IndexedModel::AllocationHints& hints) const {
			ArrayList<IndexedModel> models;

			if (!AssetLoader::loadAssets(fileName, hints, models)) {
				DEBUG_LOG(LOG_ERROR, "Static Mesh",
						"Failed to load static mesh data from: %s", fileName);
				return nullptr;
			}

			return Memory::make_shared<IndexedModel>(models[0]);
		}
	private:
};

