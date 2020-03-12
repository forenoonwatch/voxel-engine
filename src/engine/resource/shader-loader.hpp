#pragma once

#include <engine/resource/resource-loader.hpp>

#include <engine/rendering/shader.hpp>

class ShaderLoader final : public ResourceLoader<ShaderLoader, Shader> {
	public:
		Memory::SharedPointer<Shader> load(RenderContext& renderContext, 
				const char* fileName) const {
			Memory::SharedPointer<Shader> sh = Memory::make_shared<Shader>(renderContext);

			if (!sh->load(fileName)) {
				return nullptr;
			}

			return sh;
		}
	private:
};

