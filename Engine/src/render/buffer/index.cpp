
#include <GL/glew.h>

#include "index.h"

namespace Techless {

	IndexBuffer::IndexBuffer(const unsigned int* Data, unsigned int Count)
		: mCount(Count)
	{
		glGenBuffers(1, &RendererID); // generate a new buffer stored at a memory address
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, mCount * sizeof(unsigned int), Data, GL_STATIC_DRAW); // puts index information into the buffer
	}

	IndexBuffer::~IndexBuffer() 
	{
		glDeleteBuffers(1, &RendererID);
	}

	void IndexBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RendererID); // binds to that buffer and specifies the type as an element array buffer
	}

	void IndexBuffer::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // binds to that buffer and specifies the type as an element array buffer
	}
}