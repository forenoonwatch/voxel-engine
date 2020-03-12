#pragma once

#include "engine/core/array-list.hpp"
#include "engine/core/hash-map.hpp"

#include "engine/math/matrix.hpp"

#include "engine/rendering/uniform-buffer.hpp"
#include "engine/rendering/shader-storage-buffer.hpp"

#include "engine/rendering/sampler.hpp"
#include "engine/rendering/texture.hpp"
#include "engine/rendering/cube-map.hpp"

class Shader {
	public:
		inline Shader(RenderContext& context)
				: context(&context)
				, programID(0) {}

		bool load(const String& fileName, const char** feedbackVaryings = nullptr,
				uintptr numFeedbackVaryings = 0,
				uint32 varyingCaptureMode = GL_INTERLEAVED_ATTRIBS);

		void setUniformBuffer(const String& name, UniformBuffer& buffer);

		void setShaderStorageBuffer(const String& name,
				ShaderStorageBuffer& buffer);

		void setSampler(const String& name, Texture& texture,
				Sampler& sampler, uint32 textureUnit);
		void setSampler(const String& name, CubeMap& cubeMap,
				Sampler& sampler, uint32 textureUnit);

		void bindComputeTexture(Texture& texture, uint32 unit,
				uint32 access, uint32 internalFormat);

		void setInt(const String& name, int32 value);
		void setFloat(const String& name, float value);

		void setVector2f(const String& name, const Vector2f& value);
		void setVector3f(const String& name, const Vector3f& value);

		void setMatrix4f(const String& name, const Matrix4f& value);

		inline int32 getUniform(const String& name) { return uniformMap[name]; }

		inline uint32 getID() { return programID; }

		~Shader();
	private:
		NULL_COPY_AND_ASSIGN(Shader);

		RenderContext* context;
		
		uint32 programID;

		ArrayList<uint32> shaders;
		HashMap<String, int32> uniformBlockMap;
		HashMap<String, int32> samplerMap;
		HashMap<String, int32> uniformMap;

		void cleanUp();
};

