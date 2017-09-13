#pragma once
#include "Camera.h"
#include "ModelMaterials.h"

class ModelRender {
	public:
		static void draw(Camera& camera, ModelMaterials* model, glm::mat4& transform, const float delta) {
			model->getShader().start();
			Mesh mesh = model->getMesh();
			
			/* Vertex and attributes buffers */
			glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBuffer().getID());
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, model->getUVs());
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

			glBindBuffer(GL_ARRAY_BUFFER, model->getNormals());
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

			/* Uniforms */
			glUniformMatrix4fv(model->getUniform_Projection(), 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
			glUniformMatrix4fv(model->getUniform_View(), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			glUniformMatrix4fv(model->getUniform_ViewInverse(), 1, GL_FALSE, glm::value_ptr(camera.getViewMatrixInverse()));
			glUniformMatrix4fv(model->getUniform_Transform(), 1, GL_FALSE, glm::value_ptr(transform));
			glUniform1i(model->getUniform_Texture(), 0);
			//
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, model->getTexture());

			/* Index buffer */
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBuffer().getID());

			/* Draw the Mesh */
			glDrawElements(GL_TRIANGLES, mesh.getIBuffer().getIndexCount(), mesh.getIBuffer().getEnumType(), nullptr);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);

			model->getShader().stop();
		}
};