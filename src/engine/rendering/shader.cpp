#include "engine/rendering/shader.hpp"

#include "engine/core/util.hpp"

#define SHADER_INFO_LOG_SIZE	1024

static bool addShader(GLuint program, const String& text,
		GLenum type, ArrayList<uint32>& shaders);
static bool checkShaderError(uint32 shader, GLenum flag,
		bool isProgram, const String& errorMessage);

static void addShaderUniforms(GLuint program,
		HashMap<String, int32>& uniformBlockMap,
		HashMap<String, int32>& samplerMap,
		HashMap<String, int32>& uniformMap); 

bool Shader::load(const String& fileName, const char** feedbackVaryings,
		uintptr numFeedbackVaryings, uint32 varyingCaptureMode) {
	StringStream ss;
	
	if (!Util::loadFileWithLinking(ss, fileName, "#include")) {
		DEBUG_LOG(LOG_ERROR, "Shader",
				"Failed to load shader file with includes: %s",
				fileName.c_str());

		return false;
	}

	const String text = ss.str();

	if (programID != 0) {
		cleanUp();
	}

	programID = glCreateProgram();

	const String version = "#version " + context->getShaderVersion()
		+ "\n#define GLSL_VERSION " + context->getShaderVersion();

	if (text.find("CS_BUILD") != String::npos) {
		const String computeShaderText = version
			+ "\n#define CS_BUILD\n" + text;

		if (!addShader(programID, computeShaderText, GL_COMPUTE_SHADER, shaders)) {
			DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load compute shader: %s",
					fileName.c_str());

			return false;
		}
	}
	else {
		const String vertexShaderText = version
			+ "\n#define VS_BUILD\n" + text;
		const String fragmentShaderText = version
			+ "\n#define FS_BUILD\n" + text;

		if (!addShader(programID, vertexShaderText, GL_VERTEX_SHADER, shaders)) {
			DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load vertex shader: %s",
					fileName.c_str());

			return false;
		}

		if (!addShader(programID, fragmentShaderText, GL_FRAGMENT_SHADER, shaders)) {
			DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load fragment shader: %s",
					fileName.c_str());

			return false;
		}

		if (text.find("GS_BUILD") != String::npos) {
			String geomShaderText = version
				+ "\n#define GS_BUILD\n" + text;

			if (!addShader(programID, geomShaderText, GL_GEOMETRY_SHADER, shaders)) {
				DEBUG_LOG(LOG_ERROR, "Shader", "Failed to load geometry shader: %s",
						fileName.c_str());

				return false;
			}
		}
	}

	if (feedbackVaryings != nullptr && numFeedbackVaryings > 0) {
		glTransformFeedbackVaryings(programID, numFeedbackVaryings,
				feedbackVaryings, varyingCaptureMode);
	}

	glLinkProgram(programID);

	if (checkShaderError(programID, GL_LINK_STATUS, true,
				"Error linking shader program")) {
		DEBUG_LOG(LOG_ERROR, "Shader", "Error linking shader program: %s",
				fileName.c_str());

		return false;
	}

	glValidateProgram(programID);

	if (checkShaderError(programID, GL_VALIDATE_STATUS, true,
				"Invalid shader program")) {
		DEBUG_LOG(LOG_ERROR, "Shader", "Invalid shader program: %s",
				fileName.c_str());

		return false;
	}

	// TODO: add attributes
	addShaderUniforms(programID, uniformBlockMap,
			samplerMap, uniformMap);

	return true;
}

void Shader::setUniformBuffer(const String& name, UniformBuffer& buffer) {
	glUniformBlockBinding(programID, uniformBlockMap[name],
			buffer.getBlockBinding());
}

void Shader::setShaderStorageBuffer(const String& name,
		ShaderStorageBuffer& buffer) {
	glUniformBlockBinding(programID, uniformBlockMap[name],
			buffer.getBlockBinding());
}

void Shader::setSampler(const String& name, Texture& texture,
		Sampler& sampler, uint32 textureUnit) {
	context->setShader(programID);

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture.getID());
	glBindSampler(textureUnit, sampler.getID());
	glUniform1i(samplerMap[name], textureUnit);
}

void Shader::setSampler(const String& name, CubeMap& cubeMap,
		Sampler& sampler, uint32 textureUnit) {
	context->setShader(programID);

	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getID());
	glBindSampler(textureUnit, sampler.getID());
	glUniform1i(samplerMap[name], textureUnit);
}

void Shader::bindComputeTexture(Texture& texture, uint32 unit,
		uint32 access, uint32 internalFormat) {
	context->setShader(programID);
	glBindImageTexture(unit, texture.getID(), 0, false, 0,
			access, internalFormat);
}

void Shader::setInt(const String& name, int32 value) {
	context->setShader(programID);
	glUniform1i(uniformMap[name], value);
}

void Shader::setFloat(const String& name, float value) {
	context->setShader(programID);
	glUniform1f(uniformMap[name], value);
}

void Shader::setVector2f(const String& name, const Vector2f& value) {
	context->setShader(programID);
	glUniform2fv(uniformMap[name], 1, (const float*)&value);
}

void Shader::setVector3f(const String& name, const Vector3f& value) {
	context->setShader(programID);
	glUniform3fv(uniformMap[name], 1, (const float*)&value);
}

void Shader::setMatrix4f(const String& name, const Matrix4f& value) {
	context->setShader(programID);
	glUniformMatrix4fv(uniformMap[name], 1, false, (const float*)&value);
}

Shader::~Shader() {
	cleanUp();
}

inline void Shader::cleanUp() {
	for (auto& shader : shaders) {
		glDetachShader(programID, shader);
		glDeleteShader(shader);
	}

	glDeleteProgram(programID);
	context->setShader(0);

	programID = 0;

	shaders.clear();
	uniformBlockMap.clear();
	samplerMap.clear();
	uniformMap.clear();
}

static bool addShader(GLuint program, const String& text,
		GLenum type, ArrayList<uint32>& shaders) {
	uint32 shader = glCreateShader(type);

	if (shader == 0) {
		DEBUG_LOG("Shader", LOG_ERROR, "Error creating shader type %d", type);
		return false;
	}

	const GLchar* p[1] = { text.c_str() };
	GLint lengths[1] = { (GLint)text.length() };

	glShaderSource(shader, 1, p, lengths);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	if (!status) {
		GLchar infoLog[SHADER_INFO_LOG_SIZE];
		glGetShaderInfoLog(shader, SHADER_INFO_LOG_SIZE, nullptr, infoLog);

		DEBUG_LOG("Shader", LOG_ERROR, "Error compiling shader type %d: '%s'\n",
				type, infoLog);

		return false;
	}

	glAttachShader(program, shader);
	shaders.push_back(shader);

	return true;
}

static bool checkShaderError(uint32 shader, GLenum flag,
		bool isProgram, const String& errorMessage) {
	GLint status = 0;
	GLchar error[SHADER_INFO_LOG_SIZE];

	if (isProgram) {
		glGetProgramiv(shader, flag, &status);
	}
	else {
		glGetShaderiv(shader, flag, &status);
	}

	if (!status) {
		if (isProgram) {
			glGetProgramInfoLog(shader, SHADER_INFO_LOG_SIZE, nullptr, error);
		}
		else {
			glGetShaderInfoLog(shader, SHADER_INFO_LOG_SIZE, nullptr, error);
		}

		DEBUG_LOG("Shader", LOG_ERROR, "%s: '%s'\n",
				errorMessage.c_str(), error);

		return true;
	}

	return false;
}

static void addShaderUniforms(GLuint program,
		HashMap<String, int32>& uniformBlockMap,
		HashMap<String, int32>& samplerMap,
		HashMap<String, int32>& uniformMap) {
	GLint numBlocks;
	glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);

	for (int32 block = 0; block < numBlocks; ++block) {
		GLint nameLen;
		glGetActiveUniformBlockiv(program, block,
				GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

		ArrayList<GLchar> name(nameLen);
		glGetActiveUniformBlockName(program, block,
				nameLen, nullptr, &name[0]);

		String uniformBlockName((char*)&name[0], nameLen - 1);
		uniformBlockMap[uniformBlockName] = glGetUniformBlockIndex(program, &name[0]);
	}

	GLint numUniforms = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

	ArrayList<GLchar> uniformName(256);

	for (int32 uniform = 0; uniform < numUniforms; ++uniform) {
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;

		glGetActiveUniform(program, uniform, uniformName.size(),
				&actualLength, &arraySize, &type, &uniformName[0]);

		String name((char*)&uniformName[0], actualLength);

		if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE) {
			samplerMap[name] = glGetUniformLocation(program, (char*)&uniformName[0]);
		}
		else {
			uniformMap[name] = glGetUniformLocation(program, (char*)&uniformName[0]);
		}
	}
}
