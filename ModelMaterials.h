#pragma once

#include "Shader.h"
#include "VertexLoader.h"

class ModelMaterials {
	public:
		ModelMaterials(const std::string& modelName, Shader* shader) {
			std::vector<float> uvsArray, normalsArray;

			mMesh = &VertexLoader::loadMesh(modelName, uvsArray, normalsArray, textureID);
			mShader = shader;

			// VAO OpenGL 2.0
			glGenBuffers(1, &uvsID);
			glBindBuffer(GL_ARRAY_BUFFER, uvsID);
			glBufferData(GL_ARRAY_BUFFER, (uvsArray.size() * sizeof(float)), &uvsArray[0], GL_STATIC_DRAW);

			glGenBuffers(1, &normalsID);
			glBindBuffer(GL_ARRAY_BUFFER, normalsID);
			glBufferData(GL_ARRAY_BUFFER, (normalsArray.size() * sizeof(float)), &normalsArray[0], GL_STATIC_DRAW);
			//

			GLuint programID = mShader->getProgramID();
			uniform_projection = glGetUniformLocation(programID, "projectionMatrix");
			uniform_view = glGetUniformLocation(programID, "viewMatrix");
			uniform_viewInverse = glGetUniformLocation(programID, "viewMatrixInverse");
			uniform_transform = glGetUniformLocation(programID, "transformMatrix");
			uniform_textureModel = glGetUniformLocation(programID, "textureModel");
		}

	public:
		inline GLuint getUVs() { return uvsID; }
		inline GLuint getNormals() { return normalsID; }
		inline GLuint getTexture() { return textureID; }

	public:
		inline Mesh&   getMesh() { return *mMesh; }
		inline Shader& getShader() { return *mShader; }

	public:
		inline GLint getUniform_Projection() { return uniform_projection; }
		inline GLint getUniform_View() { return uniform_view; }
		inline GLint getUniform_ViewInverse() { return uniform_viewInverse; }
		inline GLint getUniform_Transform() { return uniform_transform; }
		inline GLint getUniform_Texture() { return uniform_textureModel; }

	private:
		GLuint uvsID;
		GLuint normalsID;
		GLuint textureID;

	private:
		Mesh* mMesh;
		Shader* mShader;

	private:
		GLint uniform_projection;
		GLint uniform_view;
		GLint uniform_viewInverse;
		GLint uniform_transform;
		GLint uniform_textureModel;
};