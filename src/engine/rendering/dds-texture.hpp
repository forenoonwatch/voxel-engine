#pragma once

#include "engine/core/common.hpp"

#include "engine/core/string.hpp"

#define MAKEFOURCC(a, b, c, d)                              \
                ((uint32)(uint8)(a) | ((uint32)(uint8)(b) << 8) |       \
				((uint32)(uint8)(c) << 16) | ((uint32)(uint8)(d) << 24 ))

#define MAKEFOURCCDXT(a) MAKEFOURCC('D', 'X', 'T', a)

#define FOURCC_DXT1 MAKEFOURCCDXT('1')
#define FOURCC_DXT2 MAKEFOURCCDXT('2')
#define FOURCC_DXT3 MAKEFOURCCDXT('3')
#define FOURCC_DXT4 MAKEFOURCCDXT('4')
#define FOURCC_DXT5 MAKEFOURCCDXT('5')

#define FOURCC_R16F 0x0000006F
#define FOURCC_G16R16F 0x00000070
#define FOURCC_A16B16G16R16F 0x00000071
#define FOURCC_R32F 0x00000072
#define FOURCC_G32R32F 0x00000073
#define FOURCC_A32B32G32R32F 0x00000074

class DDSTexture {
	public:
		inline DDSTexture()
				: data(nullptr) {}

		bool load(const String& fileName);

		uint32 getInternalPixelFormat() const;
		bool isCompressed() const;

		uint32 getDataSize() const;

		inline uint32 getWidth() const { return width; }
		inline uint32 getHeight() const { return height; }

		inline uint32 getMipMapCount() const { return mipMapCount; }
		inline uint32 getFourCC() const { return fourCC; }

		inline bool isCubeMap() const { return cubeMap; }

		inline const uint8* getData() const { return data; }

		~DDSTexture();
	private:
		NULL_COPY_AND_ASSIGN(DDSTexture);

		uint32 width;
		uint32 height;

		uint32 linearSize;
		uint32 mipMapCount;
		uint32 fourCC;

		bool cubeMap;

		uint8* data;

		void cleanUp();
};
