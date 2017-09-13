#pragma once
#include "Maths.h"
#include <vector>

#define NODE_TYPE_NODE 0x01
#define NODE_TYPE_ORIGIN 0x02
#define NODE_TYPE_LEAF 0x03
#define OCTREE_CHILDREN_COUNT 0x08

class Octree;

class EntityAABB {
	protected:
		Octree* m_Node;

	public:
		EntityAABB(glm::vec3 min, glm::vec3 max) {
			this->min = min;
			this->max = max;
		}

	public:
		glm::vec3 min;
		glm::vec3 max;

	public:
		inline void setNode(Octree* node) {
			this->m_Node = node;
		}
};

class Octree {
	public:
		Octree(float size, unsigned int nodeType) : m_Size(size) {
			// 1.0f is the min_Size of nodeAABB volumeSize:
			if (m_Size > 1.0f) {
				float reSize = size * 0.5f;
				m_nodeType = nodeType;
				
				for (int i = 0; i < OCTREE_CHILDREN_COUNT; i++) {
					m_Nodes[i] = new Octree(reSize, NODE_TYPE_NODE);
				}
			}
			else if (m_Size == 1.0f) {
				m_nodeType = NODE_TYPE_LEAF;
			}
		}

		virtual ~Octree() {
			if (m_nodeType != NODE_TYPE_LEAF) {
				for (int i = 0; i < OCTREE_CHILDREN_COUNT; i++) {
					delete m_Nodes[i];
				}
			}
		}

	public:
		// min and max is the AABB hard.
		//.x for un Cube 3D with size ^3.
		void addEntity(EntityAABB& aabb) {
			if (m_nodeType == NODE_TYPE_LEAF) {
				aabb.setNode(this);
				entities.push_back(aabb);
				return;
			}

			float sizeAABB = (aabb.max.x - aabb.min.x);
			float sizeChildren = (m_Size * 0.5f);

			if (sizeAABB <= (sizeChildren)) {
				if ((aabb.min.x < (sizeChildren)) && (aabb.max.x < (sizeChildren))) {
					if ((aabb.min.y < (sizeChildren)) && (aabb.max.y < (sizeChildren))) {
						if ((aabb.min.z < (sizeChildren)) && (aabb.max.z < (sizeChildren))) {
							m_Nodes[0]->addEntity(aabb);
							return;
						}
						else if ((aabb.min.z > (sizeChildren)) && (aabb.max.z > (sizeChildren))) {
							m_Nodes[1]->addEntity(aabb);
							return;
						}
					}
					else if ((aabb.min.y > (sizeChildren)) && (aabb.max.y > (sizeChildren))) {
						if ((aabb.min.z < (sizeChildren)) && (aabb.max.z < (sizeChildren))) {
							m_Nodes[2]->addEntity(aabb);
							return;
						}
						else if ((aabb.min.z > (sizeChildren)) && (aabb.max.z > (sizeChildren))) {
							m_Nodes[3]->addEntity(aabb);
							return;
						}
					}
				}
				else if ((aabb.min.x > (sizeChildren)) && (aabb.max.x > (sizeChildren))) {
					if ((aabb.min.y < (sizeChildren)) && (aabb.max.y < (sizeChildren))) {
						if ((aabb.min.z < (sizeChildren)) && (aabb.max.z < (sizeChildren))) {
							m_Nodes[4]->addEntity(aabb);
							return;
						}
						else if ((aabb.min.z > (sizeChildren)) && (aabb.max.z > (sizeChildren))) {
							m_Nodes[5]->addEntity(aabb);
							return;
						}
					}
					else if ((aabb.min.y > (sizeChildren)) && (aabb.max.y > (sizeChildren))) {
						if ((aabb.min.z < (sizeChildren)) && (aabb.max.z < (sizeChildren))) {
							m_Nodes[6]->addEntity(aabb);
							return;
						}
						else if ((aabb.min.z > (sizeChildren)) && (aabb.max.z > (sizeChildren))) {
							m_Nodes[7]->addEntity(aabb);
							return;
						}
					}
				}
				aabb.setNode(this);
				entities.push_back(aabb);
			}
			else {
				aabb.setNode(this);
				entities.push_back(aabb);
			}
		}

	public:
		Octree* m_Nodes[OCTREE_CHILDREN_COUNT];
		std::vector<EntityAABB> entities;

	private:
		char m_nodeType;
		float m_Size;
};