#pragma once

#include "includes\glm\glm.hpp"
#include "includes\glm/gtc/matrix_transform.hpp"
#include "includes\glm/gtc/type_ptr.hpp"

class Maths {
	public:
		static float baryCentric(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec2& pos) {
			float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
			float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
			float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
			float l3 = 1.0f - l1 - l2;
			return l1 * p1.y + l2 * p2.y + l3 * p3.y;
		}
		static glm::mat4 createTransformationMatrix(const glm::vec3& translate, const float rx, const float ry, const float rz, const float scale){
			glm::mat4 mTransform;
			mTransform = glm::translate(mTransform, translate);
			mTransform = glm::rotate(mTransform, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
			mTransform = glm::rotate(mTransform, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
			mTransform = glm::rotate(mTransform, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
			mTransform = glm::scale(mTransform, glm::vec3(scale, scale, scale));
			return mTransform;
		}

		static glm::mat4 createTransformationMatrix(const glm::vec3& translate, const float rx, const float ry, const float rz, const glm::vec3& scale) {
			glm::mat4 mTransform;
			mTransform = glm::translate(mTransform, translate);
			mTransform = glm::rotate(mTransform, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
			mTransform = glm::rotate(mTransform, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
			mTransform = glm::rotate(mTransform, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
			mTransform = glm::scale(mTransform, scale);
			return mTransform;
		}
};