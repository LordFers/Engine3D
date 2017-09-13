#pragma once
#include <vector>

class VertexBuffer {
	public:
		VertexBuffer(const int id) : mID(id) {
			
		}

		~VertexBuffer() {

		}

	public:
		inline int getID() const { return mID; }

	private:
		int mID;
};