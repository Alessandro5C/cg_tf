#pragma once
#ifndef __TEXTURE__
#define __TEXTURE__

#include "typedef.h"

namespace tex {

	class Texture {
	private:
		GLuint id;
		GLenum type;
		Path* path;
		GLenum slot;
	public:
		Texture(
			GLenum slot,
			string textureFileName,
			string texturePath = "D:/JC/UPC 2021-01/Comp. Graf/JackCruz/Git OpenGL/resources/textures",
			GLenum textureFilter = GL_LINEAR,
			GLenum wrapS = GL_REPEAT,
			GLenum wrapT = GL_REPEAT,
			bool initial_bind = true
		) : slot(slot), path(new Path(texturePath)), type(GL_TEXTURE_2D) {
			string filePath = path->ruta(textureFileName);

			glGenTextures(1, &id);

			if (initial_bind) Bind();
			glBindTexture(type, id);

			glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapS);
			glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapT);
			glTexParameteri(type, GL_TEXTURE_MIN_FILTER, textureFilter);
			glTexParameteri(type, GL_TEXTURE_MAG_FILTER, textureFilter);

			stbi_set_flip_vertically_on_load(true); // porque en opgl el eje Y invertio

			i32 width, height, nrChannels;
			u8* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

			if (data != nullptr) {

				GLenum format;
				if (nrChannels == 1) {
					format = GL_RED;
				}
				else if (nrChannels == 3) {
					format = GL_RGB;
				}
				else {
					format = GL_RGBA;
				}

				glTexImage2D(type, 0, format, width, height, 0,
					format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(type);
			}
			else {
				std::cerr << "Can't load texture\n";
			}

			stbi_image_free(data);
		}
		~Texture() {
			glDeleteTextures(1, &id);
		}

		void Bind() {
			glActiveTexture(slot);
			glBindTexture(type, id);
		}
		void Unbind() {
			glBindTexture(type, 0);
		}
	};
}
#endif