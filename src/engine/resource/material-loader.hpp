#pragma once

#include <engine/game/resource-loader.hpp>

#include <engine/rendering/material.hpp>

class MaterialLoader final : public ResourceLoader<MaterialLoader, Material> {
	public:
		Memory::SharedPointer<Material> load(Texture& diffuse, Texture& normal,
				Texture& material) const {
			auto mt = Memory::make_shared<Material>();
			mt->diffuse = &diffuse;
			mt->normalMap = &normal;
			mt->materialMap = &material;

			return mt;
		}
	private:
};

