#pragma once


namespace Techless {
	class IndexBuffer
	{
	public:
		IndexBuffer(const unsigned int* Data, unsigned int Count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		inline unsigned int GetCount() const { return mCount; };
	private:
		unsigned int RendererID;
		unsigned int mCount = 0;
	};
}