#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/vertex-array.hpp>

#include <engine/resource/asset-loader.hpp>

class VertexArrayLoader final
		: public ResourceLoader<VertexArrayLoader, VertexArray> {
	public:
		Memory::SharedPointer<VertexArray> load(RenderContext& renderContext,
				const char* fileName,
				const IndexedModel::AllocationHints& hints) const {
			ArrayList<IndexedModel> models;

			if (!AssetLoader::loadAssets(fileName, hints, models)) {
				DEBUG_LOG(LOG_ERROR, "Static Mesh",
						"Failed to load static mesh data from: %s", fileName);
				return nullptr;
			}

			return Memory::make_shared<VertexArray>(renderContext,
					models[0], GL_STATIC_DRAW);
		}
	private:
};

