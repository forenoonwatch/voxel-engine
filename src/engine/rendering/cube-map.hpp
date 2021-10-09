#pragma once

#include "engine/rendering/render-context.hpp"

#include "engine/rendering/bitmap.hpp"
#include "engine/rendering/dds-texture.hpp"

class CubeMap {
	public:
		inline CubeMap(RenderContext& context)
				: context(&context)
				, textureID(0) {}

		CubeMap(RenderContext& context, Bitmap* bitmaps,
				uint32 internalFormat = GL_RGB);
		CubeMap(RenderContext& context, const DDSTexture& ddsTexture);

		bool load(const String& fileName);
		bool load(const String* fileNames);

		inline uint32 getID() { return textureID; }

		inline uint32 getInternalFormat() const { return internalFormat; }

		inline bool isCompressed() const { return compressed; }
		inline bool hasMipMaps() const { return mipMaps; }

		~CubeMap();
	private:
		NULL_COPY_AND_ASSIGN(CubeMap);

		RenderContext* context;

		uint32 textureID;

		uint32 internalFormat;

		bool compressed;
		bool mipMaps;

		CubeMap(RenderContext&, uint32, bool, bool);

		void initTexture();
		void initFromDDS(const DDSTexture&);
};
