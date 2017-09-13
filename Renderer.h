#pragma once

#include "TerrainRender.h"
#include "SkyboxRender.h"
#include "ModelRender.h"
#include "Frustum.h"
#include "MD5Model.h"
#include "Octree.h"
#include <map>

class Renderer {
	public:
		Renderer(const int width, const int height) {
			terrain = new TerrainRender();
			skybox = new SkyBoxRender((float)terrain->getDepth());
			mCamera.setProjection(glm::perspective(glm::radians(45.0f), ((float)width / (float)height), 0.1f, (float)terrain->getDepth() * 4.0f));
			a.LoadModel("player", getShader("model"));

			mOctree = new Octree(8.0f, NODE_TYPE_ORIGIN);
			mOctree->addEntity(EntityAABB(glm::vec3(0.1f, 0.1f, 0.1f), glm::vec3(0.1f, 0.1f, 0.1f)));
		}

		void render(const float delta) {
			frustum.update(mCamera.getProjection() * mCamera.getViewMatrix());

			ModelRender::draw(mCamera, getModel("deadtree", "model"), Maths::createTransformationMatrix(glm::vec3(200.0f, terrain->getHeightOfTerrain(200.0f, 200.0f), 200.0f), 0.0f, 0.0f, 0.0f, 1.0f), delta);
			
			float height = terrain->getHeightOfTerrain(mCamera.getPosition().x + 2.0f, mCamera.getPosition().z + 2.0f);
			mCamera.setHeight(height + 1.0f);

			//ModelRender::draw(mCamera, getModel("darius", "model"), Maths::createTransformationMatrix(glm::vec3(mCamera.getPosition().x+2.0f, height, mCamera.getPosition().z+2.0f), 0.0f, 1.0f, 0.0f, 0.01f), delta);

			if (frustum.CubeInFrustum(0.5f, 0.5f, 0.5f, 0.5f)) {
				ModelRender::draw(mCamera, getModel("box", "model"), Maths::createTransformationMatrix(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, 1.0f), delta);
				renderBox(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 1.0f, 1.0f));
			}
			
			a.Render(mCamera, Maths::createTransformationMatrix(glm::vec3(mCamera.getPosition().x + 0.0f, height, mCamera.getPosition().z + 5.0f), -90.0f, 0.0f, 0.0f, 0.1f), delta);
			
			terrain->render(mCamera, delta);
			skybox->render(mCamera, delta);
		}

		void renderBox(const glm::vec3 &center, const glm::vec3 &dim, const glm::vec4 &clr = glm::vec4(1.f, 1.f, 1.f, 1.f)) {
			// Back "face"
			glBegin(GL_LINE_STRIP);
			glColor4f(clr.r, clr.g, clr.b, clr.a);
			glVertex3f(center.x - dim.x, center.y - dim.y, center.z - dim.z);
			glVertex3f(center.x + dim.x, center.y - dim.y, center.z - dim.z);
			glVertex3f(center.x + dim.x, center.y + dim.y, center.z - dim.z);
			glVertex3f(center.x - dim.x, center.y + dim.y, center.z - dim.z);
			glVertex3f(center.x - dim.x, center.y - dim.y, center.z - dim.z);
			glEnd();

			// Front "face"
			glBegin(GL_LINE_STRIP);
			glColor4f(clr.r, clr.g, clr.b, clr.a);
			glVertex3f(center.x - dim.x, center.y - dim.y, center.z + dim.z);
			glVertex3f(center.x + dim.x, center.y - dim.y, center.z + dim.z);
			glVertex3f(center.x + dim.x, center.y + dim.y, center.z + dim.z);
			glVertex3f(center.x - dim.x, center.y + dim.y, center.z + dim.z);
			glVertex3f(center.x - dim.x, center.y - dim.y, center.z + dim.z);
			glEnd();

			glBegin(GL_LINES);
			glColor4f(clr.r, clr.g, clr.b, clr.a);
			glVertex3f(center.x - dim.x, center.y - dim.y, center.z + dim.z);
			glVertex3f(center.x - dim.x, center.y - dim.y, center.z - dim.z);

			glVertex3f(center.x + dim.x, center.y - dim.y, center.z + dim.z);
			glVertex3f(center.x + dim.x, center.y - dim.y, center.z - dim.z);

			glVertex3f(center.x + dim.x, center.y + dim.y, center.z + dim.z);
			glVertex3f(center.x + dim.x, center.y + dim.y, center.z - dim.z);

			glVertex3f(center.x - dim.x, center.y + dim.y, center.z + dim.z);
			glVertex3f(center.x - dim.x, center.y + dim.y, center.z - dim.z);
			glEnd();

		}

	public:
		inline TerrainRender& getTerrain() { return *terrain; }
		inline Camera& getCamera() { return mCamera; }

	public:
		inline int getDepthWorld() { return terrain->getDepth(); }

	private:
		Shader* getShader(const std::string& filename) {
			if (mShaders[filename]) {
				return mShaders.at(filename);
			}

			mShaders[filename] = new Shader(filename);
			return mShaders.at(filename);
		}

		ModelMaterials* getModel(const std::string& filename, const std::string& shadername) {
			if (mModels[filename]) {
				return mModels.at(filename);
			}

			mModels[filename] = new ModelMaterials(filename, getShader(shadername));
			return mModels[filename];
		}

	private:
		Camera mCamera;
		Frustum frustum;
		MD5Model a;

		Octree* mOctree;

	private:
		TerrainRender*  terrain;
		SkyBoxRender*   skybox;

	private:
		std::map<std::string, ModelMaterials*> mModels;
		std::map<std::string, Shader*> mShaders;
};