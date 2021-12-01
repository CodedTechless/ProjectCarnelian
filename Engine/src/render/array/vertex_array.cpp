
#include <GL/glew.h>

#include "vertex_array.h"

namespace Techless {
	VertexArray::VertexArray() {
		glCreateVertexArrays(1, &RendererID);
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &RendererID);
	}

	void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer> iBuffer)
	{
		Bind();
		iBuffer->Bind();

		CurrentIndexBuffer = iBuffer;
	}

	void VertexArray::PushVertexBuffer(const std::shared_ptr<VertexBuffer> vBuffer)
	{
		Bind();
		vBuffer->Bind(); // the arrays are now linked. apply the attributes and stuff

		const auto& Layout = vBuffer->GetLayout();
		const auto& Elements = Layout.GetElements();
		
		unsigned int Offset = 0;
		for (const auto& Element : Elements)
		{
			glEnableVertexAttribArray(CurrentBufferIndex);
			glVertexAttribPointer(CurrentBufferIndex, Element.Count, (int)Element.Type, Element.Normalised ? GL_TRUE : GL_FALSE, Layout.GetStride(), (const void*)Offset);

			CurrentBufferIndex++;

			Offset += Element.Count * LayoutElement::GetSize(Element.Type);
		}

		VertexBuffers.push_back(vBuffer);
	}

	void VertexArray::Bind() const {
		glBindVertexArray(RendererID);
	}

	void VertexArray::Unbind() const {
		glBindVertexArray(0);
	}
}