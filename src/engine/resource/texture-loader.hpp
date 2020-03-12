#pragma once

#include <engine/core/string.hpp>

#include <engine/game/resource-loader.hpp>

#include <engine/rendering/texture.hpp>

#include <engine/core/util.hpp>

class TextureLoader final : public ResourceLoader<TextureLoader, Texture> {
	public:
		// TODO: std::string_view?
		Memory::SharedPointer<Texture> load(const String& fileName) const {
			const String fileExtension
					= Util::getFileExtension(fileName).to_lower();

			if (fileExtension.compare("dds") == 0) {
				DDSTexture ddsTexture;

				if (!ddsTexture.load(fileName)) {
					return nullptr;
				}

				return Memory::make_shared<Texture>(
						*static_cast<RenderContext*>(
						&Application::getInstance()), ddsTexture);
			}
			else if (fileExtension.compare("hdr") == 0) {
				DEBUG_LOG(LOG_ERROR, "Texture",
						"HDR Textures are not yet supported");

				return nullptr;
			}
			else {
				Bitmap bmp;

				if (!bmp.load(fileName)) {
					return nullptr;
				}

				return Memory::make_shared<Texture>(
						*static_cast<RenderContext*>(
						&Application::getInstance()), bmp, GL_RGBA);
			}
		}
	private:
};

