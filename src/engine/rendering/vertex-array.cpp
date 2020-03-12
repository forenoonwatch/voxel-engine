#include "engine/rendering/vertex-array.hpp"

#include "engine/core/memory.hpp"

VertexArray::VertexArray(RenderContext& context,
			const IndexedModel& model, uint32 usage)
		: context(&context)
		, arrayID(0)
		, numBuffers(model.getNumVertexComponents()
				+ model.getNumInstanceComponents() + 1)
		, numElements(model.getNumIndices())
		, instancedComponentStartIndex(model.getInstancedElementStartIndex())
		, numOwnedBuffers(numBuffers)
		, buffers(new GLuint[numBuffers])
		, bufferSizes(new uintptr[numBuffers])
		, usage(usage)
		, indexed(indexed)
		, bufferOwnership(FULLY_OWNED) {
	glGenVertexArrays(1, &arrayID);
	context.setVertexArray(arrayID);

	glGenBuffers(numBuffers, buffers);

	ArrayList<const float*> vertexData = model.getVertexData();

	if (model.getFlags() & IndexedModel::FLAG_INTERLEAVED_INSTANCES) {
		initMultiVertexSingleInstance(model.getNumVertexComponents(), 
				&vertexData[0], model.getNumVertices(),
				model.getElementSizes(), true);
	}
	else {
		initMultiVertexMultiInstance(model.getNumVertexComponents(),
				&vertexData[0], model.getNumVertices(),
				model.getElementSizes(), true);
	}

	uintptr indicesSize = numElements * sizeof(uint32);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize,
			model.getIndices(), usage);

	bufferSizes[numBuffers - 1] = indicesSize;
}

VertexArray::VertexArray(RenderContext& context,
			const IndexedModel& model, VertexArray& vertexArray)
		: context(&context)
		, arrayID(0)
		, numBuffers(vertexArray.numBuffers)
		, numElements(vertexArray.numElements)
		, instancedComponentStartIndex(vertexArray.instancedComponentStartIndex)
		, numOwnedBuffers(model.getNumInstanceComponents())
		, buffers(new GLuint[numBuffers])
		, bufferSizes(new uintptr[numBuffers])
		, usage(vertexArray.usage)
		, indexed(vertexArray.indexed)
		, bufferOwnership(SHARED_VERTEX_BUFFERS) {
	glGenVertexArrays(1, &arrayID);
	context.setVertexArray(arrayID);

	Memory::memcpy(buffers, vertexArray.buffers, numBuffers * sizeof(GLuint));

	glGenBuffers(numOwnedBuffers, buffers + instancedComponentStartIndex);

	ArrayList<const float*> vertexData = model.getVertexData();
	initMultiVertexMultiInstance(model.getNumVertexComponents(),
			&vertexData[0], model.getNumVertices(), model.getElementSizes(),
			false);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
	
	bufferSizes[numBuffers - 1] = vertexArray.bufferSizes[numBuffers - 1];
}

VertexArray::VertexArray(RenderContext& context, uint32 numBuffersIn,
			const uint32* elementSizes, uint32 numElements, uint32 usage,
			uint32 instancedElementStartIndex, bool indexed)
		: context(&context)
		, arrayID(0)
		, numBuffers(indexed ? numBuffersIn + 1 : numBuffersIn)
		, numElements(numElements)
		, instancedComponentStartIndex(instancedElementStartIndex)
		, numOwnedBuffers(numBuffers)
		, buffers(new GLuint[numBuffers])
		, bufferSizes(new uintptr[numBuffers])
		, usage(usage)
		, indexed(indexed)
		, bufferOwnership(FULLY_OWNED) {
	glGenVertexArrays(1, &arrayID);
	context.setVertexArray(arrayID);

	glGenBuffers(numBuffers, buffers);

	initEmptyArrayBuffers(numBuffers, numElements, elementSizes);

	if (indexed) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElements * sizeof(uint32), nullptr, usage);

		bufferSizes[numBuffers - 1] = numElements * sizeof(uint32);
	}
}

VertexArray::VertexArray(RenderContext& context,
			const IndexedModel& model, TransformFeedback& tfb,
			uint32 bufferNum, uint32 usage)
		: context(&context)
		, arrayID(0)
		, numBuffers(model.getNumVertexComponents() + 2)
		, numElements(model.getNumIndices())
		, instancedComponentStartIndex(model.getInstancedElementStartIndex())
		, numOwnedBuffers(numBuffers - 1)
		, buffers(new GLuint[numBuffers])
		, bufferSizes(new uintptr[numBuffers])
		, usage(usage)
		, indexed(true)
		, bufferOwnership(SHARED_INSTANCE_BUFFERS) {
	glGenVertexArrays(1, &arrayID);
	context.setVertexArray(arrayID);

	glGenBuffers(model.getNumVertexComponents(), buffers);
	glGenBuffers(1, &buffers[numBuffers - 1]);

	buffers[numBuffers - 2] = tfb.getBuffer(bufferNum);

	ArrayList<const float*> vertexData = model.getVertexData();
	initSharedBuffers(model.getNumVertexComponents(), &vertexData[0],
			model.getNumVertices(), model.getElementSizes(),
			tfb.getNumAttribs(), tfb.getAttribSizes(),
			tfb.getDataBlockSize(), tfb.getBufferSize(), true);

	uintptr indicesSize = numElements * sizeof(uint32);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize,
			model.getIndices(), usage);

	bufferSizes[numBuffers - 1] = indicesSize;
}

VertexArray::VertexArray(RenderContext& context,
			const IndexedModel& model, VertexArray& vertexArray,
			TransformFeedback& tfb, uint32 bufferNum)
		: context(&context)
		, arrayID(0)
		, numBuffers(model.getNumVertexComponents() + 2)
		, numElements(vertexArray.numElements)
		, instancedComponentStartIndex(vertexArray.instancedComponentStartIndex)
		, numOwnedBuffers(0)
		, buffers(new GLuint[numBuffers])
		, bufferSizes(new uintptr[numBuffers])
		, usage(vertexArray.usage)
		, indexed(vertexArray.indexed)
		, bufferOwnership(FULLY_SHARED) {
	glGenVertexArrays(1, &arrayID);
	context.setVertexArray(arrayID);

	Memory::memcpy(buffers, vertexArray.buffers,
			model.getNumVertexComponents() * sizeof(GLuint));

	buffers[numBuffers - 2] = tfb.getBuffer(bufferNum);
	buffers[numBuffers - 1] = vertexArray.buffers[vertexArray.numBuffers - 1];

	ArrayList<const float*> vertexData = model.getVertexData();
	initSharedBuffers(model.getNumVertexComponents(), &vertexData[0],
			model.getNumVertices(), model.getElementSizes(),
			tfb.getNumAttribs(), tfb.getAttribSizes(),
			tfb.getDataBlockSize(), tfb.getBufferSize(), false);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);
	
	bufferSizes[numBuffers - 1] = vertexArray.bufferSizes[numBuffers - 1];
}

void VertexArray::updateBuffer(uint32 bufferIndex,
		const void* data, uintptr dataSize) {
	context->setVertexArray(arrayID);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[bufferIndex]);

	if (dataSize <= bufferSizes[bufferIndex]) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, data);
	}
	else {
		bufferSizes[bufferIndex] = dataSize;
		glBufferData(GL_ARRAY_BUFFER, dataSize, data,
				bufferIndex >= instancedComponentStartIndex ? GL_DYNAMIC_DRAW 
				: usage);
	}
}

void VertexArray::updateIndices(const uint32* indices, uint32 numIndices) {
	context->setVertexArray(arrayID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[numBuffers - 1]);

	const uintptr newSize = static_cast<uintptr>(numIndices) * sizeof(uint32);

	if (newSize <= bufferSizes[numBuffers - 1]) {
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, newSize, indices);
	}
	else {
		bufferSizes[numBuffers - 1] = newSize;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, indices, usage);
	}

	numElements = numIndices;
}

VertexArray::~VertexArray() {
	switch (bufferOwnership) {
		case FULLY_OWNED:
			glDeleteBuffers(numBuffers, buffers);
			break;
		case SHARED_VERTEX_BUFFERS:
			glDeleteBuffers(numOwnedBuffers,
					buffers + instancedComponentStartIndex);
			break;
		case SHARED_INSTANCE_BUFFERS:
			glDeleteBuffers(numOwnedBuffers, buffers);
			
			if (indexed) {
				glDeleteBuffers(1, &buffers[numBuffers - 1]);
			}
			break;
	}
	
	glDeleteVertexArrays(1, &arrayID);
	context->setVertexArray(0);

	delete[] buffers;
	delete[] bufferSizes;
}

void VertexArray::initMultiVertexMultiInstance(uint32 numVertexComponents,
		const float** vertexData, uint32 numVertices,
		const uint32* vertexElementSizes, bool writeData) {
	for (uint32 i = 0, attribute = 0; i < numBuffers - 1; ++i) {
		uint32 attribUsage = usage;
		bool instancedMode = false;

		if (i >= numVertexComponents) {
			attribUsage = GL_DYNAMIC_DRAW;
			instancedMode = true;
		}

		uint32 elementSize = vertexElementSizes[i];
		const void* bufferData = instancedMode ? nullptr : vertexData[i];
		uintptr dataSize = instancedMode ? elementSize * sizeof(float)
			: elementSize * sizeof(float) * numVertices;

		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);

		if (writeData || instancedMode) {
			glBufferData(GL_ARRAY_BUFFER, dataSize, bufferData, attribUsage);
		}
		
		bufferSizes[i] = dataSize;

		initDistributedAttribute(elementSize, instancedMode, attribute);
	}
}

void VertexArray::initMultiVertexSingleInstance(uint32 numVertexComponents,
		const float** vertexData, uint32 numVertices,
		const uint32* vertexElementSizes, bool writeData) {
	uint32 attribute = 0;

	for (uint32 i = 0; i < numVertexComponents; ++i) {
		const uint32 elementSize = vertexElementSizes[i];
		const uintptr dataSize = elementSize * sizeof(float) * numVertices;

		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);

		if (writeData) {
			glBufferData(GL_ARRAY_BUFFER, dataSize, vertexData[i], usage);
		}
		
		bufferSizes[i] = dataSize;

		initDistributedAttribute(elementSize, false, attribute);
	}

	uint32 instancedDataSize = 0;

	for (uint32 i = numVertexComponents; i < numBuffers - 1; ++i) {
		instancedDataSize += vertexElementSizes[i] * sizeof(float);
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffers[numVertexComponents]);
	glBufferData(GL_ARRAY_BUFFER, instancedDataSize, nullptr, GL_DYNAMIC_DRAW);

	bufferSizes[numVertexComponents] = instancedDataSize; 

	initInterleavedAttributes(instancedDataSize,
			numBuffers - 1 - numVertexComponents,
			vertexElementSizes + numVertexComponents, true, attribute);
}

void VertexArray::initEmptyArrayBuffers(uint32 numVertexComponents,
		uint32 numVertices, const uint32* vertexElementSizes) {
	for (uint32 i = 0, attribute = 0; i < numBuffers; ++i) {
		uint32 attribUsage = usage;
		bool instancedMode = false;

		if (i >= numVertexComponents) {
			attribUsage = GL_DYNAMIC_DRAW;
			instancedMode = true;
		}

		uint32 elementSize = vertexElementSizes[i];
		uintptr dataSize = instancedMode ? elementSize * sizeof(float)
			: elementSize * sizeof(float) * numVertices;

		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
		glBufferData(GL_ARRAY_BUFFER, dataSize, nullptr, attribUsage);

		bufferSizes[i] = dataSize;

		initDistributedAttribute(elementSize, instancedMode, attribute);
	}
}

void VertexArray::initSharedBuffers(uint32 numVertexComponents,
		const float** vertexData, uint32 numVertices,
		const uint32* vertexElementSizes, uint32 numInstanceComponents,
		const uint32* instanceElementSizes, uint32 instanceDataSize,
		uint32 instanceBufferSize, bool writeVertexComponents) {
	uint32 attribute = 0;

	for (uint32 i = 0; i < numVertexComponents; ++i) {
		const uint32 elementSize = vertexElementSizes[i];
		const uintptr dataSize = elementSize * sizeof(float) * numVertices;

		glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);

		if (writeVertexComponents) {
			glBufferData(GL_ARRAY_BUFFER, dataSize, vertexData[i], usage);
		}

		bufferSizes[i] = dataSize;

		initDistributedAttribute(elementSize, false, attribute);
	}

	glBindBuffer(GL_ARRAY_BUFFER, buffers[numVertexComponents]);
	bufferSizes[numVertexComponents] = instanceBufferSize; 

	initInterleavedAttributes(instanceDataSize, numInstanceComponents,
			instanceElementSizes, true, attribute);
}

inline void VertexArray::initDistributedAttribute(uint32 elementSize,
		bool instancedMode, uint32& attribute) {
	const uint32 elementSizeDiv = elementSize / 4;
	const uint32 elementSizeRem = elementSize % 4;

	for (uint32 j = 0; j < elementSizeDiv; ++j) {
		glEnableVertexAttribArray(attribute);
		glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_FALSE,
				elementSize * sizeof(float),
				reinterpret_cast<const void*>(j * 4 * sizeof(float)));

		if (instancedMode) {
			glVertexAttribDivisor(attribute, 1);
		}

		++attribute;
	}

	if (elementSizeRem != 0) {
		glEnableVertexAttribArray(attribute);
		glVertexAttribPointer(attribute, elementSize, GL_FLOAT, GL_FALSE,
				elementSize * sizeof(float),
				reinterpret_cast<const void*>(elementSizeDiv * 4
				* sizeof(float)));

		if (instancedMode) {
			glVertexAttribDivisor(attribute, 1);
		}

		++attribute;
	}
}

inline void VertexArray::initInterleavedAttributes(uint32 blockSize,
		uint32 numElements, const uint32* elementSizes, bool instancedMode,
		uint32& attribute) {
	for (uint32 i = 0, offset = 0; i < numElements; ++i) {
		const uint32 elementSize = elementSizes[i];
		const uint32 elementSizeDiv = elementSize / 4;
		const uint32 elementSizeRem = elementSize % 4;

		for (uint32 j = 0; j < elementSizeDiv; ++j) {
			glEnableVertexAttribArray(attribute);
			glVertexAttribPointer(attribute, 4, GL_FLOAT, GL_FALSE,
					blockSize, reinterpret_cast<const void*>(offset));

			if (instancedMode) {
				glVertexAttribDivisor(attribute, 1);
			}

			offset += 4 * sizeof(float);
			++attribute;
		}

		if (elementSizeRem != 0) {
			glEnableVertexAttribArray(attribute);
			glVertexAttribPointer(attribute, elementSizeRem, GL_FLOAT,
					GL_FALSE, blockSize,
					reinterpret_cast<const void*>(offset));
			
			if (instancedMode) {
				glVertexAttribDivisor(attribute, 1);
			}

			offset += elementSizeRem * sizeof(float);
			++attribute;
		}
	}
}

