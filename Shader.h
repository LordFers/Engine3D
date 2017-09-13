#pragma once
#include <GLEW\glew.h>
#include <fstream>
#include <vector>
#include <string>

class Shader {
	public:
		Shader(const std::string& filename) {
			// Create the shaders
			mVertexID = glCreateShader(GL_VERTEX_SHADER);
			mFragmentID = glCreateShader(GL_FRAGMENT_SHADER);

			std::string vertexShader = "resources/shaders/" + filename + ".vert";
			std::string fragmentShader = "resources/shaders/" + filename + ".frag";

			// Read the Vertex Shader code from the file
			std::string VertexShaderCode;
			std::ifstream VertexShaderStream(vertexShader, std::ios::in);
			if (VertexShaderStream.is_open()) {
				std::string Line = "";
				while (getline(VertexShaderStream, Line))
					VertexShaderCode += "\n" + Line;
				VertexShaderStream.close();
			}
			else {
				printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertexShader.c_str());
				getchar();
				return;
			}

			// Read the Fragment Shader code from the file
			std::string FragmentShaderCode;
			std::ifstream FragmentShaderStream(fragmentShader, std::ios::in);
			if (FragmentShaderStream.is_open()) {
				std::string Line = "";
				while (getline(FragmentShaderStream, Line))
					FragmentShaderCode += "\n" + Line;
				FragmentShaderStream.close();
			}

			GLint Result = GL_FALSE;
			int InfoLogLength;

			// Compile Vertex Shader
			printf("Compiling shader : %s\n", vertexShader.c_str());
			char const * VertexSourcePointer = VertexShaderCode.c_str();
			glShaderSource(mVertexID, 1, &VertexSourcePointer, NULL);
			glCompileShader(mVertexID);

			// Check Vertex Shader
			glGetShaderiv(mVertexID, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(mVertexID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
				glGetShaderInfoLog(mVertexID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
				printf("%s\n", &VertexShaderErrorMessage[0]);
			}

			// Compile Fragment Shader
			printf("Compiling shader : %s\n", fragmentShader.c_str());
			char const * FragmentSourcePointer = FragmentShaderCode.c_str();
			glShaderSource(mFragmentID, 1, &FragmentSourcePointer, NULL);
			glCompileShader(mFragmentID);

			// Check Fragment Shader
			glGetShaderiv(mFragmentID, GL_COMPILE_STATUS, &Result);
			glGetShaderiv(mFragmentID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
				glGetShaderInfoLog(mFragmentID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
				printf("%s\n", &FragmentShaderErrorMessage[0]);
			}

			// Link the program
			printf("Linking program\n");
			mProgramID = glCreateProgram();
			glAttachShader(mProgramID, mVertexID);
			glAttachShader(mProgramID, mFragmentID);
			glLinkProgram(mProgramID);
			glValidateProgram(mProgramID);

			// Check the program
			glGetProgramiv(mProgramID, GL_LINK_STATUS, &Result);
			glGetProgramiv(mProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
			if (InfoLogLength > 0) {
				std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
				glGetProgramInfoLog(mProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
				printf("%s\n", &ProgramErrorMessage[0]);
			}
		}

		~Shader() { }

	public:
		void start() {
			glUseProgram(mProgramID);
		}

		void stop() {
			glUseProgram(0);
		}

		void cleanUp() {
			glUseProgram(0);
			glDetachShader(mProgramID, mVertexID);
			glDetachShader(mProgramID, mFragmentID);

			glDeleteShader(mVertexID);
			glDeleteShader(mFragmentID);
			glDeleteProgram(mProgramID);
		}

	public:
		inline GLuint getProgramID() { return mProgramID; }
		inline GLuint getVertexID() { return mVertexID; }
		inline GLuint getFragmentID() { return mFragmentID; }

	private:
		GLuint mProgramID;
		GLuint mVertexID;
		GLuint mFragmentID;
};