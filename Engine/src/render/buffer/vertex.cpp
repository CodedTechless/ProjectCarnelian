
#include "vertex.h"

namespace Techless {

	VertexBuffer::VertexBuffer(unsigned int Size)
	{
		glGenBuffers(1, &RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, Size, nullptr, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::VertexBuffer(const void* Data, unsigned int Size)
	{
		glGenBuffers(1, &RendererID); // generate a new buffer stored at a memory address
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferData(GL_ARRAY_BUFFER, Size, Data, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::~VertexBuffer() 
	{
		glDeleteBuffers(1, &RendererID);
	}

	void VertexBuffer::Set(const void* Data, unsigned int Size) 
	{
		glBindBuffer(GL_ARRAY_BUFFER, RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, Size, Data); // puts data into the array to be drawn
	}

	void VertexBuffer::Bind() const 
	{
		glBindBuffer(GL_ARRAY_BUFFER, RendererID); // binds to that buffer and specifies the type as an array buffer
	}

	void VertexBuffer::Unbind() const 
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void VertexBuffer::SetBufferLayout(const VertexBufferLayout& Layout)
	{
		aLayout = Layout;
	}

	// Buffer Layout
	VertexBufferLayout::VertexBufferLayout(std::initializer_list<LayoutElement> Init)
		: bStride(0), Elements(Init) 
	{
		for (auto& Element : Elements) {
			bStride += LayoutElement::GetSize(Element.Type) * Element.Count;
		}
	}

	void VertexBufferLayout::SetLayout(std::vector<LayoutElement> NewElements) 
	{
		for (auto& Element : NewElements)
		{
			Elements.push_back(Element);
			bStride += LayoutElement::GetSize(Element.Type) * Element.Count;
		}
	}
}