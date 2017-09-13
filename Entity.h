#pragma once
#include "Mesh.h"
#include "Maths.h"

//enum ENTITY {
//	E_Unknown = 0, E_Mesh, E_Light, E_Particle
//};

class Entity {
	public:
		~Entity() {}

	public:
		virtual void create(Mesh* mesh, glm::mat4 transform = glm::mat4(1.0f)) = 0;

	public:
		virtual inline Mesh& getMesh() = 0;
		virtual inline glm::vec3& getPosition() = 0;
		virtual inline glm::mat4& getTransform() = 0;
		virtual inline glm::mat4& getTransform_Plus_Rotation(const float angle, glm::vec3& axis) = 0;

	public:
		virtual inline void setPosition(glm::vec3& position) = 0;
		virtual inline void setTransformation(glm::mat4& transform) = 0;
		virtual inline void setRotation(const float angle, glm::vec3& axis) = 0;
		virtual inline void setScale(glm::vec3& axis) = 0;
		//void Rotate->

	protected:
		Mesh* mMesh;
		
	protected:
		glm::vec3 mPosition;
		glm::mat4 mTransform;

};