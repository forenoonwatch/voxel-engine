#include "engine/rendering/cube-map.hpp"

#include "engine/core/util.hpp"

CubeMap::CubeMap(RenderContext& context, Bitmap* bitmaps,
			uint32 internalFormat)
		: CubeMap(context, internalFormat, false, false) {
	for (uint32 i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, bitmaps[i].getWidth(),
				bitmaps[i].getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bitmaps[i].getPixels());
	}
}

CubeMap::CubeMap(RenderContext& context, const DDSTexture& ddsTexture)
		: CubeMap(context, ddsTexture.getInternalPixelFormat(),
				ddsTexture.isCompressed(), ddsTexture.getMipMapCount() > 1) {
	initFromDDS(ddsTexture);
}

bool CubeMap::load(const String& fileName) {
	const String fileExtension = Util::getFileExtension(fileName).to_lower();

	if (fileExtension.compare("dds") == 0) {
		DDSTexture ddsTexture;

		if (!ddsTexture.load(fileName)) {
			return false;
		}

		internalFormat = ddsTexture.getInternalPixelFormat();
		compressed = ddsTexture.isCompressed();
		mipMaps = ddsTexture.getMipMapCount() > 1;

		initTexture();
		initFromDDS(ddsTexture);

		return true;
	}
	else if (fileExtension.compare("hdr") == 0) {
		DEBUG_LOG(LOG_ERROR, "Cube Map",
				"HDR equirectangular maps are not currently supported: %s",
				fileName.c_str());

		return false;
	}
	else {
		/*Bitmap bmp;

		if (!bmp.load(fileName)) {
			return false;
		}

		internalFormat = GL_RGB; // TODO: deduce IF from bmp
		compressed = false;
		mipMaps = false;

		initTexture();*/

		DEBUG_LOG(LOG_ERROR, "Cube Map",
				"Equirectangular maps are not currently supported: %s",
				fileName.c_str());

		return false;
	}
}

bool CubeMap::load(const String* fileNames) {
	String fileExtension;

	internalFormat = GL_RGB;
	compressed = false;
	mipMaps = false;

	for (uint32 i = 0; i < 6; ++i) {
		fileExtension = Util::getFileExtension(fileNames[i]).to_lower();

		if (fileExtension.compare("dds") == 0) {
			DEBUG_LOG(LOG_ERROR, "Cube Map",
					"DDS Textures for single cube sides are not currently supported: %s",
					fileNames[i].c_str());

			return false;
		}
		else if (fileExtension.compare("hdr") == 0) {
			DEBUG_LOG(LOG_ERROR, "Cube Map",
					"HDR Textures for single cube sides are not currently supported: %s",
					fileNames[i].c_str());

			return false;
		}
		else {
			Bitmap bmp;

			if (!bmp.load(fileNames[i])) {
				return false;
			}

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, bmp.getWidth(),
					bmp.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, bmp.getPixels());
		}
	}

	return true;
}

CubeMap::~CubeMap() {
	glDeleteTextures(1, &textureID);
}

inline CubeMap::CubeMap(RenderContext& context, uint32 internalFormat,
			bool compressed, bool mipMaps)
		: context(&context)
		, textureID(0)
		, internalFormat(internalFormat)
		, compressed(compressed)
		, mipMaps(mipMaps) {
	initTexture();
}

inline void CubeMap::initTexture() {
	if (textureID != 0) {
		glDeleteTextures(1, &textureID);
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

inline void CubeMap::initFromDDS(const DDSTexture& ddsTexture) {
	if (compressed) {
		const uint32 blockSize = (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;

		uint32 offset = 0;

		for (uint32 i = 0; i < 6; ++i) {
			uint32 w = ddsTexture.getWidth();
			uint32 h = ddsTexture.getHeight();

			for (uint32 level = 0; level < ddsTexture.getMipMapCount()
					&& (w || h); ++level) {
				uint32 size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;

				glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level,
						internalFormat, w, h, 0, size, ddsTexture.getData() + offset);

				offset += size;
				w /= 2;
				h /= 2;
			}
		}
	}
	else {
		uint32 blockSize, dataType;

		if (internalFormat == GL_RGBA32F) {
			blockSize = 16;
			dataType = GL_FLOAT;
		}
		else {
			blockSize = 8;
			dataType = GL_HALF_FLOAT;
		}

		uint32 offset = 0;
		
		for (uint32 i = 0; i < 6; ++i) {
			uint32 w = ddsTexture.getWidth();
			uint32 h = ddsTexture.getHeight();

			for (uint32 level = 0; level < ddsTexture.getMipMapCount()
					&& (w || h); ++level) {
				uint32 size = w * h * blockSize;

				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, level,
						internalFormat, w, h, 0, GL_RGBA, dataType,
						ddsTexture.getData() + offset);

				offset += size;
				w /= 2;
				h /= 2;
			}
		}
	}
}
