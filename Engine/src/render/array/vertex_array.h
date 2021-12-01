#pragma once

#include <engineincl.h>

#include "render/buffer/index.h"
#include "render/buffer/vertex.h"

namespace Techless {
	class VertexArray {
	public:
		VertexArray();
		~VertexArray();

		void SetIndexBuffer(const std::shared_ptr<IndexBuffer> iBuffer);
		void PushVertexBuffer(const std::shared_ptr<VertexBuffer> vBuffer);

//		void SetBufferLayout(const VertexBuffer& vBuffer, const VertexBufferLayout& Layout);
//		VertexBufferLayout SetBufferLayout(const VertexBuffer& vBuffer, std::initializer_list<LayoutElement> LayoutInit);

		void Bind() const;
		void Unbind() const;

	private:
		std::shared_ptr<IndexBuffer> CurrentIndexBuffer;
		std::vector<std::shared_ptr<VertexBuffer>> VertexBuffers;

	private:
		unsigned int CurrentBufferIndex = 0;
		unsigned int RendererID;
	};
}
