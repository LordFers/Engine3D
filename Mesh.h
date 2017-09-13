#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"

class Mesh {
	public:
		Mesh(const VertexBuffer& VBO, const IndexBuffer& IBO) : mVBuffer(VBO), mIBuffer(IBO) {
			
		}

		~Mesh() {

		}

	public:
		inline VertexBuffer& getVBuffer() { return mVBuffer; }
		inline IndexBuffer& getIBuffer() { return mIBuffer; }

	private:
		VertexBuffer mVBuffer;
		IndexBuffer  mIBuffer;
};