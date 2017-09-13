#pragma once
#include "TerrainEntity.h"
#include "Shader.h"
#include "VertexLoader.h"
#include "Camera.h"

class TerrainRender {
	public:
		TerrainRender() {
			loadTerrain();
			loadMaterials();
		}

		void render(Camera& camera, const float delta) {
			glCullFace(GL_FRONT);
			//
			Mesh mesh = mTerrainEntity->getMesh();
			/* Apply shaders */
			mShader->start();

			/* Vertex buffer */
			glBindBuffer(GL_ARRAY_BUFFER, mesh.getVBuffer().getID());

			/* Attributes */
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);

			/*  */
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(sizeof(float) * 3));

			/* Uniforms */
			glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
			glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(mTerrainEntity->getTransform()));
			glUniform1f(uniform_worldSize, (float)mDepth);
			glUniform1f(uniform_delta, delta);

			glUniform1i(uniform_backgroundTexture, 0);
			glUniform1i(uniform_textureR, 1);
			glUniform1i(uniform_textureG, 2);
			glUniform1i(uniform_textureB, 3);
			glUniform1i(uniform_blendMap, 4);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, backgroundTexture);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textureR);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, textureG);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, textureB);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, blendMap);

			/* Index buffer */
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBuffer().getID());

			/* Draw the Mesh */
			glDrawElements(GL_TRIANGLES, mesh.getIBuffer().getIndexCount(), mesh.getIBuffer().getEnumType(), nullptr);

			/* Disable attributes*/
			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);

			mShader->stop();
			glCullFace(GL_BACK);
		}

	public:
		inline int getDepth() { return mDepth; }
		void setLOD(int level) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mTerrainEntity->getMesh().getIBuffer().getID());
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (mIndex.at(level).size() * sizeof(unsigned int)), &(mIndex.at(level).at(0)), GL_STATIC_DRAW);
		}

	private:
		void loadMaterials() {
			mShader = new Shader("terrain");
			GLuint programID = mShader->getProgramID();
			uniform_projection = glGetUniformLocation(programID, "projectionMatrix");
			uniform_view = glGetUniformLocation(programID, "viewMatrix");
			uniform_transform = glGetUniformLocation(programID, "transformMatrix");
			uniform_worldSize = glGetUniformLocation(programID, "worldSize");
			uniform_delta = glGetUniformLocation(programID, "delta");

			/* samplers2D */
			uniform_backgroundTexture = glGetUniformLocation(programID, "backgroundTexture");
			uniform_textureR = glGetUniformLocation(programID, "textureR");
			uniform_textureG = glGetUniformLocation(programID, "textureG");
			uniform_textureB = glGetUniformLocation(programID, "textureB");
			uniform_blendMap = glGetUniformLocation(programID, "blendMap");

			/* Load textures to Terrain */
			backgroundTexture = TextureLoader::LoadPNG("resources/textures/1.png");
			textureR = TextureLoader::LoadPNG("resources/textures/2.png");
			textureG = TextureLoader::LoadPNG("resources/textures/3.png");
			textureB = TextureLoader::LoadPNG("resources/textures/4.png");
			blendMap = TextureLoader::LoadPNG("resources/textures/blendMap.png");
		}

	private:
		void loadTerrain() {
			loadHeightMap("resources/textures/heightmap.png");
			glm::vec3 normals;
			//+1 para ahorrar cada 5 vértices, 1.
			for (float z = 0; z < mDepth; z++) {
				for (float x = 0; x < mDepth; x++) {
					mVertex.emplace_back(x);
					mVertex.emplace_back(getHeight((int)x + 0, (int)z + 0));
					mVertex.emplace_back(z);

					getNormal((int)x + 0, (int)z + 0, normals);
					mVertex.emplace_back(normals.x);
					mVertex.emplace_back(normals.y);
					mVertex.emplace_back(normals.z);

					mVertex.emplace_back((x + 1.0f));
					mVertex.emplace_back(getHeight((int)x + 1, (int)z + 0));
					mVertex.emplace_back(z);

					getNormal((int)x + 1, (int)z, normals);
					mVertex.emplace_back(normals.x);
					mVertex.emplace_back(normals.y);
					mVertex.emplace_back(normals.z);

					mVertex.emplace_back(x);
					mVertex.emplace_back(getHeight((int)x + 0, (int)z + 1));
					mVertex.emplace_back((z + 1.0f));

					getNormal((int)x, (int)z + 1, normals);
					mVertex.emplace_back(normals.x);
					mVertex.emplace_back(normals.y);
					mVertex.emplace_back(normals.z);

					mVertex.emplace_back((x + 1.0f));
					mVertex.emplace_back(getHeight((int)x + 1, (int)z + 1));
					mVertex.emplace_back((z + 1.0f));

					getNormal((int)x + 1, (int)z + 1, normals);
					mVertex.emplace_back(normals.x);
					mVertex.emplace_back(normals.y);
					mVertex.emplace_back(normals.z);
				}
			}

			mDepth--;

			int j = 0;
			mIndex.resize(4 + 1);
			for (int z = 0; z < mDepth; z++) {
				for (int x = 0; x < mDepth; x++) {
					mIndex.at(0).emplace_back(j + 0);
					mIndex.at(0).emplace_back(j + 1);
					mIndex.at(0).emplace_back(j + 3);

					mIndex.at(0).emplace_back(j + 0);
					mIndex.at(0).emplace_back(j + 3);
					mIndex.at(0).emplace_back(j + 2);
					j += 4;
				}
				j += 4;
			}
			
			/* Hago una recursión sobre el conjunto de índices anterior, es más fácil de re-acomodarlos de esta forma, y más rápido. */
			int preDepthLOD = mDepth;
			for (int nLevel = 1; nLevel <= 4; nLevel++) {
				int nT = (int)pow(2, nLevel);
				int depthReSize = (mDepth / nT);

				for (int z = 0, j = 0; z < depthReSize; z++) {
					for (int x = 0; x < depthReSize; x++) {
						mIndex.at(nLevel).emplace_back(mIndex.at(nLevel - 1).at(j + 0));
						mIndex.at(nLevel).emplace_back(mIndex.at(nLevel - 1).at(j + 1 + 6));
						mIndex.at(nLevel).emplace_back(mIndex.at(nLevel - 1).at(j + 2 + preDepthLOD * 6 + 6));

						mIndex.at(nLevel).emplace_back(mIndex.at(nLevel - 1).at(j + 3));
						mIndex.at(nLevel).emplace_back(mIndex.at(nLevel - 1).at(j + 4 + preDepthLOD * 6 + 6));
						mIndex.at(nLevel).emplace_back(mIndex.at(nLevel - 1).at(j + 5 + preDepthLOD * 6));
						j += 6 * 2;
					}
					j += preDepthLOD * 6;
				}
				preDepthLOD = depthReSize;
			}

			mTerrainEntity = new TerrainEntity();
			mTerrainEntity->create(&VertexLoader::loadMesh(mVertex, mIndex.at(0)), Maths::createTransformationMatrix(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 0.0f, 0.0f, glm::vec3(1.0f, 1.0f, 1.0f)));
		}

		void loadHeightMap(const char *filename) {
			int width, height;
			unsigned char* data = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);
			mDepth = height;
			mHeightMap.resize(width * height * 3);
			for (int i = 0; i < width * height * 3; i++) {
				mHeightMap.at(i) = data[i];
			}

			delete data;
		}

		//The number 3 is the equivalent to RGB mode, and 4 RGBA.
		float getHeight(const int x, const int z) {
			int depth = (int)sqrt(mHeightMap.size() / 3);
			if (x < 0 || x >= depth || z < 0 || z >= depth) {
				return 0.0f;
			}

			int aux = (z * depth + x) * 3;
			float height = (float)getIntFromColor(mHeightMap.at(aux + 0), mHeightMap.at(aux + 1), mHeightMap.at(aux + 2));
			int max_pixel_color = 256 * 256 * 256;

			height += (float)((float)max_pixel_color / 2.0f);
			height /= (float)((float)max_pixel_color / 2.0f);
			height *= 20.0f;

			return height;
		}

		void getNormal(const int x, const int z, glm::vec3& normal) {
			int depth = (int)sqrt(mHeightMap.size() / 3);
			float sx = (float)getHeight(x < depth - 1 ? x + 1 : x, z) - (float)getHeight(x ? x - 1: x, z);
			if (x == 0 || x == depth - 1)
				sx *= 2;

			float sz = (float)getHeight(x, z < depth - 1 ? z + 1 : z) - (float)getHeight(x, z ? z - 1 : z);
			if (z == 0 || z == depth - 1)
				sz *= 2;

			normal = glm::vec3(-sx, 2.0f, sz);
			normal = glm::normalize(normal);
		}

		int getIntFromColor(int Red, int Green, int Blue) {
			Red = (Red << 16) & 0x00FF0000;
			Green = (Green << 8) & 0x0000FF00;
			Blue = Blue & 0x000000FF;
			return (0xFF000000 | Red | Green | Blue);
		}

	public:
		float getHeightOfTerrain(float x, float z) {
			int depth = (int)sqrt(mHeightMap.size() / 3);
			
			int gridX = (int)std::floorf(x);
			int gridZ = (int)std::floorf(z);

			if (gridX < 0 || gridX >= (depth - 1))
				return 0.0f;

			if (gridZ < 0 || gridZ >= (depth - 1))
				return 0.0f;

			float vX = abs(x - (float)gridX);
			float vZ = abs(z - (float)gridZ);

			float height = 0.0f;

			if (vX <= (1.0f - vZ)) {
				height = Maths::baryCentric(glm::vec3(0.0f, getHeight(gridX, gridZ + 1), 1.0f),
					glm::vec3(1.0f, getHeight(gridX + 1, gridZ), 0.0f), glm::vec3(0.0f, getHeight(gridX, gridZ), 0.0f), glm::vec2(vX, vZ));
			}
			else {
				height = Maths::baryCentric(glm::vec3(0.0f, getHeight(gridX, gridZ + 1), 1.0f),
					glm::vec3(1.0f, getHeight(gridX + 1, gridZ + 1), 1.0f), glm::vec3(1.0f, getHeight(gridX + 1, gridZ), 0.0f), glm::vec2(vX, vZ));
			}

			return height;
;
		}

		void setHeight(const int x, const int z, const int radius) {
			int depth = (int)sqrt(mHeightMap.size() / 3);
			if (x < 0 || z < 0 || x + radius >= depth || z + radius >= depth)
				return;

			for (int gZ = z; gZ < z + radius+1; gZ++) {
				for (int gX = x; gX < x + radius+1; gX++) {
					int index = (gZ * depth + gX) * 3;
					if (mHeightMap.at(index + 0) >= 255)
						break;
					if (mHeightMap.at(index + 1) >= 255)
						break;
					if (mHeightMap.at(index + 2) >= 255)
						break;

					mHeightMap.at(index + 0) += 1;
					mHeightMap.at(index + 1) += 1;
					mHeightMap.at(index + 2) += 1;
				}
			}

			int offZ = 0, offX = 0;
			if (z > 0) {
				offZ--;
			}

			if (x > 0) {
				offX--;
			}

			//+1 y -1 para los vértices vecinos.
			for (int gZ = z + offZ; gZ < z + radius + 1; gZ++) {
				for (int gX = x + offX; gX < x + radius + 1; gX++) {
					int index = (gZ * depth + gX) * 24; // 24 = struct of vertex;
					// gl_Vertex[0]
					mVertex.at(1 + index) = getHeight(gX, gZ);

					// gl_Vertex[1]
					mVertex.at(7 + index) = getHeight(gX + 1, gZ);

					// gl_Vertex[2]
					mVertex.at(13 + index) = getHeight(gX, gZ + 1);

					// gl_Vertex[3]
					mVertex.at(19 + index) = getHeight(gX + 1, gZ + 1);
				}
			}

			if ((z + offZ) > 0) {
				offZ--;
			}

			if ((x + offX) > 0) {
				offX--;
			}

			int offMaxZ = 0, offMaxX = 0;
			if ((z + radius + 1) < depth) {
				offMaxZ++;
			}

			if ((x + radius + 1) < depth) {
				offMaxX++;
			}

			// Reload normals after of load the vertex in your list.
			glm::vec3 tempNormal;
			for (int gZ = z + offZ; gZ < z + radius + 1 + offMaxZ; gZ++) {
				for (int gX = x + offX; gX < x + radius + 1 + offMaxX; gX++) {
					int index = (gZ * depth + gX) * 24; // 24 = struct of vertex;
					// gl_Normal[0]
					getNormal(gX, gZ, tempNormal);
					mVertex.at(1 + 2 + index) = tempNormal.x;
					mVertex.at(1 + 3 + index) = tempNormal.y;
					mVertex.at(1 + 4 + index) = tempNormal.z;

					// gl_Normal[1]
					getNormal(gX + 1, gZ, tempNormal);
					mVertex.at(7 + 2 + index) = tempNormal.x;
					mVertex.at(7 + 3 + index) = tempNormal.y;
					mVertex.at(7 + 4 + index) = tempNormal.z;

					// gl_Normal[2]
					getNormal(gX, gZ + 1, tempNormal);
					mVertex.at(13 + 2 + index) = tempNormal.x;
					mVertex.at(13 + 3 + index) = tempNormal.y;
					mVertex.at(13 + 4 + index) = tempNormal.z;

					// gl_Normal[3]
					getNormal(gX + 1, gZ + 1, tempNormal);
					mVertex.at(19 + 2 + index) = tempNormal.x;
					mVertex.at(19 + 3 + index) = tempNormal.y;
					mVertex.at(19 + 4 + index) = tempNormal.z;
				}
			}

			glBindBuffer(GL_ARRAY_BUFFER, mTerrainEntity->getMesh().getVBuffer().getID());
			glBufferData(GL_ARRAY_BUFFER, (mVertex.size() * sizeof(float)), &mVertex[0], GL_STATIC_DRAW);
		}

	private:
		int mDepth;

	private:
		GLint uniform_projection;
		GLint uniform_view;
		GLint uniform_transform;
		GLint uniform_worldSize;
		GLint uniform_delta;

		GLint uniform_backgroundTexture;
		GLint uniform_blendMap;
		GLint uniform_textureR;
		GLint uniform_textureG;
		GLint uniform_textureB;

	private:
		GLuint backgroundTexture;
		GLuint blendMap;
		GLuint textureR;
		GLuint textureG;
		GLuint textureB;

	private:
		Entity* mTerrainEntity;
		Shader* mShader;

	private:
		std::vector<float> mVertex;
		std::vector<std::vector<unsigned int>> mIndex;
		std::vector<unsigned char> mHeightMap;
};