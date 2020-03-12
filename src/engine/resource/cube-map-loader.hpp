#pragma once

#include <engine/core/string.hpp>

#include <engine/game/resource-loader.hpp>

#include <engine/core/application.hpp>

class CubeMapLoader final
		: public ResourceLoader<CubeMapLoader, CubeMap> {
	public:
		Memory::SharedPointer<CubeMap> load(const String* fileNames,
				uint32 numFiles) const {
			Memory::SharedPointer<CubeMap> cm = Memory::make_shared<CubeMap>(
					*static_cast<RenderContext*>(&Application::getInstance()));

			if (numFiles == 1) {
				if (!cm->load(*fileNames)) {
					return nullptr;
				}

				return cm;
			}
			else {
				if (!cm->load(fileNames)) {
					return nullptr;
				}

				return cm;
			}
		}
	private:
};

