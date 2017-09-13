#pragma once
#include <vector>

class IndexBuffer {
	public:
		IndexBuffer(const int id, const GLenum enumType, const unsigned int count)
			: mID(id), mEnumType(enumType), mIndexCount(count)
		{

		}

		~IndexBuffer() {

		}

		/*IndexBuffer operator=(const IndexBuffer& other) {
			if (this != &other) {
				//(*this) = other;
				return other;
			}
		}*/

	public:
		inline int getID() const { return mID; }
		inline GLenum getEnumType() const { return mEnumType; }
		inline unsigned int getIndexCount() const { return mIndexCount; }

	public:
		inline void setEnumType(GLenum enumType) { mEnumType = enumType; }

	private:
		int mID;
		unsigned int mIndexCount;
		GLenum mEnumType;
};