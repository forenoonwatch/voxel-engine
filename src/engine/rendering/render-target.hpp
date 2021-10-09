#pragma once

#include "engine/rendering/texture.hpp"
#include "engine/rendering/render-buffer.hpp"

class RenderTarget {
	public:
		inline RenderTarget(RenderContext& context, uint32 width,
					uint32 height)
				: context(&context)
				, bufferID(0)
				, width(width) 
				, height(height) {}

		RenderTarget(RenderContext& context, Texture& texture,
				uint32 width, uint32 height, uint32 attachmentType,
				uint32 attachmentNumber = 0);
		RenderTarget(RenderContext& context, Texture& texture,
				uint32 attachmentType, uint32 attachmentNumber = 0);

		void clear(uint32 flags);
		void setDrawBuffers(uint32 numBuffers);

		void drawTo(RenderTarget& target, uint32 mask = GL_COLOR_BUFFER_BIT,
				uint32 filter = GL_NEAREST);

		void addTextureTarget(Texture& texture, uint32 attachmentType,
				uint32 attachmentNumber = 0);
		void addRenderBuffer(RenderBuffer& buffer, uint32 attachmentType,
				uint32 attachmentNumber = 0);

		inline void resize(uint32 width, uint32 height);

		inline uint32 getID() { return bufferID; }

		inline uint32 getWidth() const { return width; }
		inline uint32 getHeight() const { return height; }

		~RenderTarget();
	private:
		NULL_COPY_AND_ASSIGN(RenderTarget);

		RenderContext* context;
		uint32 bufferID;

		uint32 width;
		uint32 height;
};

inline void RenderTarget::resize(uint32 width, uint32 height) {
	this->width = width;
	this->height = height;
}
