#pragma once
#include "Entity.h"

class ModelEntity : public Entity {
	public:
		virtual void create(Mesh* mesh, glm::mat4 transform = glm::mat4(1.0f)) override {
			mMesh = mesh;
			mTransform = transform;
		}

	public:
		virtual inline Mesh& getMesh() override {
			return *mMesh;
		}

		virtual inline glm::vec3& getPosition() override {
			return mPosition;
		}

		virtual inline glm::mat4& getTransform() override {
			return mTransform;
		}

		virtual inline glm::mat4& getTransform_Plus_Rotation(const float angle, glm::vec3& axis) override {
			mTransform = glm::rotate(mTransform, glm::radians(angle), axis);
			return mTransform;
		}

	public:
		virtual inline void setPosition(glm::vec3& position) override {
			mPosition = position;
		}

		virtual inline void setTransformation(glm::mat4& transform) override {
			mTransform = transform;
		}

		virtual inline void setRotation(const float angle, glm::vec3& axis) override {
			mTransform = glm::rotate(mTransform, glm::radians(angle), axis);
		}

		virtual inline void setScale(glm::vec3& axis) override {
			mTransform = glm::scale(mTransform, axis);
		}

	protected:
		Mesh* mMesh;

	protected:
		glm::vec3 mPosition;
		glm::mat4 mTransform;
};