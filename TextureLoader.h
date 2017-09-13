#pragma once
#include <GLEW\glew.h>
#include "includes\SOIL\SOIL.h"
#include <fstream>

typedef union PixelInfo
{
	std::uint32_t Colour;
	struct
	{
		std::uint8_t R, G, B, A;
	};
} *PPixelInfo;

class TextureLoader {	
public:
	static GLuint LoadPNG(const char * file_name) {
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		int width, height;
		unsigned char* image = SOIL_load_image(file_name, &width, &height, 0, SOIL_LOAD_AUTO);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

		return texture;
	}

	static GLuint LoadSkyBox(const char * A, const char * B, const char * C, const char * D, const char * E, const char * F) {
		return SOIL_load_OGL_cubemap(A, B, C, D, E, F, SOIL_LOAD_RGBA, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	}
};