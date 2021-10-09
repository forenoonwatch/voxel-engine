#include "engine/rendering/gaussian-blur.hpp"

#define NUM_PASSES 10
#define LOCAL_BLOCK_SIZE 16

GaussianBlur::GaussianBlur(RenderContext& context, Shader& blurShader,
			Texture& blurTarget)
		: context(&context)
		, blurShader(blurShader)
		, blurTarget(blurTarget)
		, buffer(context, blurTarget.getWidth(),
				blurTarget.getHeight(), GL_RGBA32F) {}

void GaussianBlur::update() {
	horizontal = true;

	blurShader.bindComputeTexture(blurTarget, 0, GL_READ_WRITE, GL_RGBA32F);
	blurShader.bindComputeTexture(buffer, 1, GL_READ_WRITE, GL_RGBA32F);

	for (uint32 i = 0; i < NUM_PASSES; ++i) {
		blurShader.setInt("horizontal", horizontal);

		context->compute(blurShader, blurTarget.getWidth() / LOCAL_BLOCK_SIZE,
				blurTarget.getHeight() / LOCAL_BLOCK_SIZE);
		context->awaitFinish();

		horizontal = !horizontal;
	}
}

void GaussianBlur::resize(uint32 width, uint32 height) {
	buffer.resize(width, height);
}

