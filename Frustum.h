#pragma once
#include "Maths.h"

class Frustum {
	public:
		enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
		glm::vec4 planes[6];
		
		void update(glm::mat4 matrix) {
			planes[LEFT].x = matrix[0].w + matrix[0].x;
			planes[LEFT].y = matrix[1].w + matrix[1].x;
			planes[LEFT].z = matrix[2].w + matrix[2].x;
			planes[LEFT].w = matrix[3].w + matrix[3].x;

			planes[RIGHT].x = matrix[0].w - matrix[0].x;
			planes[RIGHT].y = matrix[1].w - matrix[1].x;
			planes[RIGHT].z = matrix[2].w - matrix[2].x;
			planes[RIGHT].w = matrix[3].w - matrix[3].x;

			planes[TOP].x = matrix[0].w - matrix[0].y;
			planes[TOP].y = matrix[1].w - matrix[1].y;
			planes[TOP].z = matrix[2].w - matrix[2].y;
			planes[TOP].w = matrix[3].w - matrix[3].y;

			planes[BOTTOM].x = matrix[0].w + matrix[0].y;
			planes[BOTTOM].y = matrix[1].w + matrix[1].y;
			planes[BOTTOM].z = matrix[2].w + matrix[2].y;
			planes[BOTTOM].w = matrix[3].w + matrix[3].y;

			planes[BACK].x = matrix[0].w + matrix[0].z;
			planes[BACK].y = matrix[1].w + matrix[1].z;
			planes[BACK].z = matrix[2].w + matrix[2].z;
			planes[BACK].w = matrix[3].w + matrix[3].z;

			planes[FRONT].x = matrix[0].w - matrix[0].z;
			planes[FRONT].y = matrix[1].w - matrix[1].z;
			planes[FRONT].z = matrix[2].w - matrix[2].z;
			planes[FRONT].w = matrix[3].w - matrix[3].z;

			for (unsigned int i = 0; i < 6; i++) {
				float length = sqrtf(planes[i].x * planes[i].x + planes[i].y * planes[i].y + planes[i].z * planes[i].z);
				planes[i] /= length;
			}
		}

		bool checkSphere(glm::vec3 pos, float radius) {
			for (unsigned int i = 0; i < 6; i++) {
				if ((planes[i].x * pos.x) + (planes[i].y * pos.y) + (planes[i].z * pos.z) + planes[i].w <= -radius) {
					return false;
				}
			}
			return true;
		}

		bool CubeInFrustum(float x, float y, float z, float size) {
			for (int i = 0; i < 6; i++) {
				if (planes[i].x * (x - size) + planes[i].y * (y - size) + planes[i].z * (z - size) + planes[i].w > 0)
					continue;
				if (planes[i].x * (x + size) + planes[i].y * (y - size) + planes[i].z * (z - size) + planes[i].w > 0)
					continue;
				if (planes[i].x * (x - size) + planes[i].y * (y + size) + planes[i].z * (z - size) + planes[i].w > 0)
					continue;
				if (planes[i].x * (x + size) + planes[i].y * (y + size) + planes[i].z * (z - size) + planes[i].w > 0)
					continue;
				if (planes[i].x * (x - size) + planes[i].y * (y - size) + planes[i].z * (z + size) + planes[i].w > 0)
					continue;
				if (planes[i].x * (x + size) + planes[i].y * (y - size) + planes[i].z * (z + size) + planes[i].w > 0)
					continue;
				if (planes[i].x * (x - size) + planes[i].y * (y + size) + planes[i].z * (z + size) + planes[i].w > 0)
					continue;
				if (planes[i].x * (x + size) + planes[i].y * (y + size) + planes[i].z * (z + size) + planes[i].w > 0)
					continue;

				return false;
			}
			return true;
		}

	/*public:
		Frustum() {

		}

		Frustum(const glm::mat4 &matrix) {
			calcPlanes(matrix);
		}

		virtual ~Frustum() {

		}

		enum Plane {
			Right = 0,
			Left,
			Bottom,
			Top,
			Far,
			Near
		};

		glm::vec4 getPlane(Plane p) const { return planes[p]; }

		void calcPlanes(const glm::mat4 &matrix) {
			planes[Right] = glm::vec4(matrix[0][3] - matrix[0][0],
				matrix[1][3] - matrix[1][0],
				matrix[2][3] - matrix[2][0],
				matrix[3][3] - matrix[3][0]);

			planes[Left] = glm::vec4(matrix[0][3] + matrix[0][0],
				matrix[1][3] + matrix[1][0],
				matrix[2][3] + matrix[2][0],
				matrix[3][3] + matrix[3][0]);

			planes[Bottom] = glm::vec4(matrix[0][3] + matrix[0][1],
				matrix[1][3] + matrix[1][1],
				matrix[2][3] + matrix[2][1],
				matrix[3][3] + matrix[3][1]);

			planes[Top] = glm::vec4(matrix[0][3] - matrix[0][1],
				matrix[1][3] - matrix[1][1],
				matrix[2][3] - matrix[2][1],
				matrix[3][3] - matrix[3][1]);

			planes[Far] = glm::vec4(matrix[0][3] - matrix[0][2],
				matrix[1][3] - matrix[1][2],
				matrix[2][3] - matrix[2][2],
				matrix[3][3] - matrix[3][2]);

			planes[Near] = glm::vec4(matrix[0][3] + matrix[0][2],
				matrix[1][3] + matrix[1][2],
				matrix[2][3] + matrix[2][2],
				matrix[3][3] + matrix[3][2]);
			// Normalize them
			for (int i = 0; i < 6; i++) {
				float invl = sqrt(planes[i].x * planes[i].x +
					planes[i].y * planes[i].y +
					planes[i].z * planes[i].z);
				planes[i] /= invl;
			}
		}

		int halfPlaneTest(const glm::vec3 &p, const glm::vec3 &normal, float offset) const {
			float dist = glm::dot(p, normal) + offset;
			if (dist > 0.0f) // Point is in front of plane
				return 1;
			else if (dist < 0.0f) // Point is behind plane
				return 0;
			return 2; // Point is on plane
		}

		int isBoxInFrustum(const glm::vec3 &origin, const glm::vec3 &halfDim) const {
			static const glm::vec3 cornerOffsets[] = {
				glm::vec3(-1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, -1.0f,  1.0f),
				glm::vec3(-1.0f,  1.0f, -1.0f),
				glm::vec3(-1.0f,  1.0f,  1.0f),
				glm::vec3( 1.0f, -1.0f, -1.0f),
				glm::vec3( 1.0f, -1.0f,  1.0f),
				glm::vec3( 1.0f,  1.0f, -1.0f),
				glm::vec3( 1.0f,  1.0f,  1.0f)
			};
			int ret = 1;
			for (int i = 0; i < 6; i++) {
				glm::vec3 planeNormal = glm::vec3(planes[i]);
				int idx = vectorToIndex(planeNormal);
				glm::vec3 testPoint = origin + halfDim * cornerOffsets[idx];

				if (halfPlaneTest(testPoint, planeNormal, planes[i].w) == 0) {
					ret = 0;
					break;
				}

				idx = vectorToIndex(-planeNormal);
				testPoint = origin + halfDim * cornerOffsets[idx];

				if (halfPlaneTest(testPoint, planeNormal, planes[i].w) == 0) {
					ret |= 2;
				}
			}
			return ret;
		}

	private:
		glm::vec4 planes[6];

		inline int vectorToIndex(const glm::vec3 &v) const {
			int idx = 0;
			if (v.z >= 0) idx |= 1;
			if (v.y >= 0) idx |= 2;
			if (v.x >= 0) idx |= 4;
			return idx;
		}*/
};