#pragma once
#include <GLEW\glew.h>
#include <sstream>
#include <iostream>

#include "Mesh.h"
#include "TextureLoader.h"
#include "Maths.h"

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

class VertexLoader {
	public:
		template <typename T>
		static Mesh& loadMesh(std::vector<float>& vertex, std::vector<T>& index) {
			return *(new Mesh(createVBO(vertex), createIBO(index)));
		}

		static Mesh& loadMeshTerrain(const std::string& filename) {
			
			
		}

		static Mesh& loadMesh(const std::string& filename, std::vector<float>& uvsArray, std::vector<float>& normalsArray, GLuint& textureID) {
			std::ifstream in("resources/models/" + filename + "/" + filename + ".obj", std::ios::in);
			if (!in) {
				std::cout << "Cannot open " << filename << std::endl; exit(1);
			}

			std::vector<float> vertices;
			std::vector<glm::vec2> uvs;
			std::vector<glm::vec3> normals;

			std::vector<unsigned int> indices;
			std::vector<unsigned int> elementsUVs;
			std::vector<unsigned int> elementsNormals;

			std::string line;

			while (getline(in, line)) {
				std::vector<std::string> currentLine;
				split(line, ' ', currentLine);

				if (line.substr(0, 2) == "v ") {
					vertices.push_back(std::stof(currentLine.at(1)));
					vertices.push_back(std::stof(currentLine.at(2)));
					vertices.push_back(std::stof(currentLine.at(3)));
				}
				else if (line.substr(0, 3) == "vt ") {
					uvs.push_back(glm::vec2(std::stof(currentLine.at(1)), std::stof(currentLine.at(2))));
				}
				else if (line.substr(0, 3) == "vn ") {
					normals.push_back(glm::vec3(std::stof(currentLine.at(1)), std::stof(currentLine.at(2)), std::stof(currentLine.at(3))));
				}
				else if (line.substr(0, 2) == "f ") {
					std::vector<std::string> vecString;
					split(line, ' ', vecString);

					std::vector<std::string> vec1;
					split(vecString.at(1), '/', vec1);

					std::vector<std::string> vec2;
					split(vecString.at(2), '/', vec2);

					std::vector<std::string> vec3;
					split(vecString.at(3), '/', vec3);

					if (uvsArray.size() < 1) {
						int count = vertices.size() / 3;
						uvsArray.resize(count * 2);
						normalsArray.resize(count * 3);
					}

					processVertex(vec1, indices, uvs, normals, uvsArray, normalsArray);
					processVertex(vec2, indices, uvs, normals, uvsArray, normalsArray);
					processVertex(vec3, indices, uvs, normals, uvsArray, normalsArray);
				}
			}

			std::string fileTexture = "resources/models/" + filename + "/" + filename + ".png";
			textureID = TextureLoader::LoadPNG(fileTexture.c_str());

			in.close();
			return VertexLoader::loadMesh(vertices, indices);
		}

	private:
		static void processVertex(std::vector<std::string>& vertexData, std::vector<unsigned int>& indices,
			std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<float>& uvsArray, std::vector<float>& normalsArray) {
			int currentVertexPointer = std::stoi(vertexData.at(0)) - 1;
			indices.push_back(currentVertexPointer);

			glm::vec2 currentUVs = uvs.at(std::stoi(vertexData.at(1)) - 1);
			uvsArray.at(currentVertexPointer * 2) = currentUVs.x;
			uvsArray.at(currentVertexPointer * 2 + 1) = 1 - currentUVs.y;

			glm::vec3 currentNormals = normals.at(std::stoi(vertexData.at(2)) - 1);
			normalsArray.at(currentVertexPointer * 3) = currentNormals.x;
			normalsArray.at(currentVertexPointer * 3 + 1) = currentNormals.y;
			normalsArray.at(currentVertexPointer * 3 + 2) = currentNormals.z;
		}

	private:
		static VertexBuffer& createVBO(std::vector<float>& vertex) {
			unsigned int vboID;
			glGenBuffers(1, &vboID);
			glBindBuffer(GL_ARRAY_BUFFER, vboID);
			glBufferData(GL_ARRAY_BUFFER, (vertex.size() * sizeof(float)), &vertex[0], GL_STATIC_DRAW);
			return *(new VertexBuffer(vboID));
		}

		template <typename T>
		static IndexBuffer& createIBO(std::vector<T>& index) {
			unsigned int iboID;
			glGenBuffers(1, &iboID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (index.size() * sizeof(T)), &index[0], GL_STATIC_DRAW);
			
			GLenum enumType;
			switch (sizeof(T)) {
				case sizeof(unsigned short):
					enumType = GL_UNSIGNED_SHORT;
					break;

				case sizeof(unsigned int):
					enumType = GL_UNSIGNED_INT;
					break;
			}

			return *(new IndexBuffer(iboID, enumType, index.size()));
		}
};