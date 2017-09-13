#pragma once
#include "Camera.h"
#include "Shader.h"
#include "TextureLoader.h"
#include "VertexLoader.h"

class SkyBoxRender {
	private:
		float vertices[36*3] = { -1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			1.0f,  1.0f, -1.0f,
			1.0f,  1.0f,  1.0f,
			1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			1.0f, -1.0f,  1.0f };

	public:
		SkyBoxRender(const float scale) {
			loadMaterials();
		}

		void render(Camera& camera, const float delta) {
			mShader->start();
			glm::mat4 sky_viewMatrix = camera.getViewMatrix();

			sky_viewMatrix[3][0] = 0;
			sky_viewMatrix[3][1] = 0;
			sky_viewMatrix[3][2] = 0;

			//Apply vboID
			glBindBuffer(GL_ARRAY_BUFFER, vboID);

			/* Attributes */
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

			/* Uniforms */
			glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
			glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(sky_viewMatrix));
			glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(mTransform));
			glUniform1i(uniform_cubemap, 0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, textureSkybox);

			//Depth Mask
			glDepthMask(GL_FALSE);

			/* Draw the Mesh */
			glDrawArrays(GL_TRIANGLES, 0, 36*3);

			//Depth Mask
			glDepthMask(GL_TRUE);
			glDisableVertexAttribArray(0);

			mShader->stop();
		}

	private:
		void loadMaterials() {
			textureSkybox = TextureLoader::LoadSkyBox("resources/textures/right.png", "resources/textures/left.png", "resources/textures/top.png", 
													  "resources/textures/bottom.png", "resources/textures/front.png", "resources/textures/back.png");

			unsigned int ID;
			glGenBuffers(1, &ID);
			glBindBuffer(GL_ARRAY_BUFFER, ID);
			glBufferData(GL_ARRAY_BUFFER, (36 * 3 * sizeof(float)), &vertices[0], GL_STATIC_DRAW);
			
			this->vboID = ID;
			this->mTransform = Maths::createTransformationMatrix(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 300.0f);

			mShader = new Shader("skybox");

			GLuint programID = mShader->getProgramID();
			uniform_projection = glGetUniformLocation(programID, "projectionMatrix");
			uniform_view = glGetUniformLocation(programID, "viewMatrix");
			uniform_transform = glGetUniformLocation(programID, "transformMatrix");
			uniform_cubemap = glGetUniformLocation(programID, "cubeMap");
		}

	private:
		Shader* mShader;

	private:
		glm::mat4 mTransform; //transform because delete the entity.

	private:
		GLint uniform_projection;
		GLint uniform_view;
		GLint uniform_transform;
		GLint uniform_cubemap;

	private:
		GLuint textureSkybox;

	private:
		GLuint vboID;
};