#pragma once
#include "MD5Animation.h"

class MD5Model {
	public:
		MD5Model() : m_iMD5Version(-1)
			, m_iNumJoints(0)
			, m_iNumMeshes(0)
			, m_bHasAnimation(false)
			{ }

		virtual ~MD5Model() { }

	protected:
		typedef std::vector<glm::vec3> PositionBuffer;
		typedef std::vector<glm::vec3> NormalBuffer;
		typedef std::vector<glm::vec2> Tex2DBuffer;
		typedef std::vector<GLuint> IndexBuffer;

		struct Vertex {
			glm::vec3		m_Pos;
			glm::vec3		m_Normal;
			glm::vec2		m_Tex0;
			int				m_StartWeight;
			int				m_WeightCount;
		};
		typedef std::vector<Vertex> VertexList;

		struct Triangle {
			int				m_Indices[3];
		};
		typedef std::vector<Triangle> TriangleList;

		struct Weight {
			int             m_JointID;
			float           m_Bias;
			glm::vec3       m_Pos;
		};
		typedef std::vector<Weight> WeightList;

		struct Joint {
			std::string     m_Name;
			int             m_ParentID;
			glm::vec3       m_Pos;
			glm::quat       m_Orient;
		};
		typedef std::vector<Joint> JointList;

		struct md5Mesh {
			std::string     m_Shader;
			VertexList      m_Verts;
			TriangleList    m_Tris;
			WeightList      m_Weights;
			GLuint          m_TexID;

			PositionBuffer  m_PositionBuffer;
			NormalBuffer    m_NormalBuffer;
			Tex2DBuffer     m_Tex2DBuffer;
			IndexBuffer     m_IndexBuffer;
		};
		typedef std::vector<md5Mesh> MeshList;

	private:
		void RemoveQuotes(std::string& str) {
			size_t n;
			while ((n = str.find('\"')) != std::string::npos) str.erase(n, 1);
		}

		void ComputeQuatW(glm::quat& quat) {
			float t = 1.0f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
			if (t < 0.0f)
			{
				quat.w = 0.0f;
			}
			else
			{
				quat.w = -sqrtf(t);
			}
		}

		void IgnoreLine(std::istream& file, int length) {
			file.ignore(length, '\n');
		}

		std::streampos fileSize(const char* filePath) {

			std::streampos fsize = 0;
			std::ifstream file(filePath, std::ios::binary);

			fsize = file.tellg();
			file.seekg(0, std::ios::end);
			fsize = file.tellg() - fsize;
			file.close();

			return fsize;
		}

		void PrepareMesh(md5Mesh& mesh) {
			mesh.m_PositionBuffer.clear();
			mesh.m_Tex2DBuffer.clear();

			// Compute vertex positions
			for (unsigned int i = 0; i < mesh.m_Verts.size(); ++i) {
				Vertex& vert = mesh.m_Verts[i];

				vert.m_Pos = glm::vec3(0.0f, 0.0f, 0.0f);
				vert.m_Normal = glm::vec3(0.0f, 0.0f, 0.0f);

				// Sum the position of the weights
				for (int j = 0; j < vert.m_WeightCount; ++j) {
					Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
					Joint& joint = m_Joints[weight.m_JointID];

					// Convert the weight position from Joint local space to object space
					glm::vec3 rotPos = joint.m_Orient * weight.m_Pos;
					vert.m_Pos += (joint.m_Pos + rotPos) * weight.m_Bias;
					
				}

				mesh.m_PositionBuffer.push_back(vert.m_Pos);
				mesh.m_Tex2DBuffer.push_back(vert.m_Tex0);
			}
		}

		bool PrepareMesh(md5Mesh& mesh, const MD5Animation::FrameSkeleton& skel)
		{
			for (unsigned int i = 0; i < mesh.m_Verts.size(); ++i)
			{
				const Vertex& vert = mesh.m_Verts[i];
				glm::vec3& pos = mesh.m_PositionBuffer[i];
				glm::vec3& normal = mesh.m_NormalBuffer[i];

				pos = glm::vec3(0.0f);
				normal = glm::vec3(0.0f);

				for (int j = 0; j < vert.m_WeightCount; ++j)
				{
					const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
					const MD5Animation::SkeletonJoint& joint = skel.m_Joints[weight.m_JointID];

					glm::vec3 rotPos = joint.m_Orient * weight.m_Pos;
					pos += (joint.m_Pos + rotPos) * weight.m_Bias;

					normal += (joint.m_Orient * vert.m_Normal) * weight.m_Bias;
				}
			}
			return true;
		}

		void PrepareNormals(md5Mesh& mesh)
		{
			mesh.m_NormalBuffer.clear();

			// Loop through all triangles and calculate the normal of each triangle
			for (unsigned int i = 0; i < mesh.m_Tris.size(); ++i)
			{
				glm::vec3 v0 = mesh.m_Verts[mesh.m_Tris[i].m_Indices[0]].m_Pos;
				glm::vec3 v1 = mesh.m_Verts[mesh.m_Tris[i].m_Indices[1]].m_Pos;
				glm::vec3 v2 = mesh.m_Verts[mesh.m_Tris[i].m_Indices[2]].m_Pos;

				glm::vec3 normal = glm::cross(v2 - v0, v1 - v0);

				mesh.m_Verts[mesh.m_Tris[i].m_Indices[0]].m_Normal += normal;
				mesh.m_Verts[mesh.m_Tris[i].m_Indices[1]].m_Normal += normal;
				mesh.m_Verts[mesh.m_Tris[i].m_Indices[2]].m_Normal += normal;
			}

			// Now normalize all the normals
			for (unsigned int i = 0; i < mesh.m_Verts.size(); ++i)
			{
				Vertex& vert = mesh.m_Verts[i];

				glm::vec3 normal = glm::normalize(vert.m_Normal);
				mesh.m_NormalBuffer.push_back(normal);

				// Reset the normal to calculate the bind-pose normal in joint space
				vert.m_Normal = glm::vec3(0);

				// Put the bind-pose normal into joint-local space
				// so the animated normal can be computed faster later
				for (int j = 0; j < vert.m_WeightCount; ++j)
				{
					const Weight& weight = mesh.m_Weights[vert.m_StartWeight + j];
					const Joint& joint = m_Joints[weight.m_JointID];
					vert.m_Normal += (normal * joint.m_Orient) * weight.m_Bias;
				}
			}
		}

		bool LoadAnim(const std::string& filename, const std::string& animation) {
			if (m_Animation.LoadAnimation(filename, animation))
			{
				// Check to make sure the animation is appropriate for this model
				m_bHasAnimation = CheckAnimation(m_Animation);
			}

			return m_bHasAnimation;
		}

		bool CheckAnimation(const MD5Animation& animation) const
		{
			if (m_iNumJoints != animation.GetNumJoints())
			{
				return false;
			}

			// Check to make sure the joints match up
			for (unsigned int i = 0; i < m_Joints.size(); ++i)
			{
				const Joint& meshJoint = m_Joints[i];
				const MD5Animation::JointInfo& animJoint = animation.GetJointInfo(i);

				if (meshJoint.m_Name != animJoint.m_Name ||
					meshJoint.m_ParentID != animJoint.m_ParentID)
				{
					return false;
				}
			}

			return true;
		}

	public:
		void LoadModel(const std::string& path, Shader* shader) {
			std::string filename = "resources/models/" + path + "/" + path + ".md5mesh";

			if (!std::ifstream(filename)) {
				std::cerr << "MD5Model::LoadModel: Failed to find file: " << filename << std::endl;
				return;
			}

			std::ifstream file(filename);
			std::string param("");
			std::string junk;

			file >> param;

			int fileLength = (int)fileSize(filename.c_str());
			
			while (!file.eof()) {
				if (param == "MD5Version") {
					file >> m_iMD5Version;
					assert(m_iMD5Version == 10);
				}
				else if (param == "commandline") {
					IgnoreLine(file, fileLength);
				}
				else if (param == "numJoints") {
					file >> m_iNumJoints;
					m_Joints.reserve(m_iNumJoints);
				}
				else if (param == "numMeshes") {
					file >> m_iNumMeshes;
					m_Meshes.reserve(m_iNumMeshes);
				}
				else if (param == "joints")
				{
					Joint joint;
					file >> junk; // Read the '{' character
					for (int i = 0; i < m_iNumJoints; ++i)
					{
						file >> joint.m_Name >> joint.m_ParentID >> junk
							>> joint.m_Pos.x >> joint.m_Pos.y >> joint.m_Pos.z >> junk >> junk
							>> joint.m_Orient.x >> joint.m_Orient.y >> joint.m_Orient.z >> junk;

						RemoveQuotes(joint.m_Name);
						ComputeQuatW(joint.m_Orient);

						m_Joints.push_back(joint);
						// Ignore everything else on the line up to the end-of-line character.
						IgnoreLine(file, fileLength);
					}
					file >> junk; // Read the '}' character
				}
				else if (param == "mesh")
				{
					md5Mesh mesh;
					int numVerts, numTris, numWeights;

					file >> junk; // Read the '{' character
					file >> param;
					while (param != "}")  // Read until we get to the '}' character
					{
						if (param == "shader")
						{
							file >> mesh.m_Shader;
							RemoveQuotes(mesh.m_Shader);

							//ARREGLAR ESTO, NO SE BIEN QUE MODIFIQUE EN LA LECTURA DEL PATH DE LA TEXTURA.
							std::string textureFile = "resources/models/" + path + "/" + mesh.m_Shader;

							mesh.m_TexID = SOIL_load_OGL_texture(textureFile.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
							file.ignore(fileLength, '\n'); // Ignore everything else on the line
						}
						else if (param == "numverts")
						{
							file >> numVerts;               // Read in the vertices
							IgnoreLine(file, fileLength);
							for (int i = 0; i < numVerts; ++i)
							{
								Vertex vert;

								file >> junk >> junk >> junk                    // vert vertIndex (
									>> vert.m_Tex0.x >> vert.m_Tex0.y >> junk  //  s t )
									>> vert.m_StartWeight >> vert.m_WeightCount;

								IgnoreLine(file, fileLength);

								mesh.m_Verts.push_back(vert);
								mesh.m_Tex2DBuffer.push_back(vert.m_Tex0);
							}
						}
						else if (param == "numtris")
						{
							file >> numTris;
							IgnoreLine(file, fileLength);
							for (int i = 0; i < numTris; ++i)
							{
								Triangle tri;
								file >> junk >> junk >> tri.m_Indices[0] >> tri.m_Indices[1] >> tri.m_Indices[2];

								IgnoreLine(file, fileLength);

								mesh.m_Tris.push_back(tri);
								mesh.m_IndexBuffer.push_back((GLuint)tri.m_Indices[0]);
								mesh.m_IndexBuffer.push_back((GLuint)tri.m_Indices[1]);
								mesh.m_IndexBuffer.push_back((GLuint)tri.m_Indices[2]);
							}
						}
						else if (param == "numweights")
						{
							file >> numWeights;
							IgnoreLine(file, fileLength);
							for (int i = 0; i < numWeights; ++i)
							{
								Weight weight;
								file >> junk >> junk >> weight.m_JointID >> weight.m_Bias >> junk
									>> weight.m_Pos.x >> weight.m_Pos.y >> weight.m_Pos.z >> junk;

								IgnoreLine(file, fileLength);
								mesh.m_Weights.push_back(weight);
							}
						}
						else
						{
							IgnoreLine(file, fileLength);
						}

						file >> param;
					}

					PrepareMesh(mesh);
					PrepareNormals(mesh);
					
					m_Meshes.push_back(mesh);
				}

				file >> param;
		}

		assert(m_Joints.size() == m_iNumJoints);
		assert(m_Meshes.size() == m_iNumMeshes);

		if (!LoadAnim(path, "run")) {
			std::cerr << "MD5Model::LoadAnimation: Failed to find file: " << filename << std::endl;
			//return;
		}

		m_Shader = shader;
		GLuint programID = m_Shader->getProgramID();
		uniform_projection = glGetUniformLocation(programID, "projectionMatrix");
		uniform_view = glGetUniformLocation(programID, "viewMatrix");
		uniform_viewInverse = glGetUniformLocation(programID, "viewMatrixInverse");
		uniform_transform = glGetUniformLocation(programID, "transformMatrix");
		uniform_textureModel = glGetUniformLocation(programID, "textureModel");
	}

	void Render(Camera& camera, glm::mat4& transform, float delta) {
		Update(delta / 1000.0f);

		// Render the meshes
		m_Shader->start();

		glCullFace(GL_FRONT);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		for (unsigned int i = 0; i < m_Meshes.size(); ++i) {
			md5Mesh& mesh = m_Meshes[i];
			
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.m_PositionBuffer[0]));
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, &(mesh.m_Tex2DBuffer[0]));
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, &(mesh.m_NormalBuffer[0]));

			/* Uniforms */
			glUniformMatrix4fv(uniform_projection, 1, GL_FALSE, glm::value_ptr(camera.getProjection()));
			glUniformMatrix4fv(uniform_view, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
			glUniformMatrix4fv(uniform_viewInverse, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrixInverse()));
			glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(transform));
			glUniform1i(uniform_textureModel, 0);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh.m_TexID);
			
			glDrawElements(GL_TRIANGLES, mesh.m_IndexBuffer.size(), GL_UNSIGNED_INT, &(mesh.m_IndexBuffer[0]));
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glCullFace(GL_BACK);
		m_Shader->stop();
	}

	void Update(float fDeltaTime) {
		if (m_bHasAnimation)
		{
			m_Animation.Update(fDeltaTime);
			const MD5Animation::FrameSkeleton& skeleton = m_Animation.GetSkeleton();

			for (unsigned int i = 0; i < m_Meshes.size(); ++i)
			{
				PrepareMesh(m_Meshes[i], skeleton);
			}
		}
	}

	private:
		int                 m_iMD5Version;
		int                 m_iNumJoints;
		int                 m_iNumMeshes;

		bool                m_bHasAnimation;

		JointList           m_Joints;
		MeshList            m_Meshes;
		std::vector<Mesh>	m_Mesh;
		MD5Animation        m_Animation;
		Shader*				m_Shader;

	private:
		GLint uniform_projection;
		GLint uniform_view;
		GLint uniform_viewInverse;
		GLint uniform_transform;
		GLint uniform_textureModel;
};