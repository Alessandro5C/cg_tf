#pragma once
#ifndef __MODEL_H__
#define __MODEL_H__

#include <typedef.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h> // C++ importer interface
#include <assimp/postprocess.h>

#include "mesh.h"
#include "files.h"

u32 TextureFromFile(const std::string& fileName, i32 param = GL_LINEAR) {
	u32 texture;

	glGenTextures(1, &texture);

	i32 w, h, nrChannels;

	//stbi_set_flip_vertically_on_load(true); // porque en opgl el eje Y invertido
	u8* data = stbi_load(fileName.c_str(), &w, &h, &nrChannels, 0);
	if (data == nullptr) {
		std::cerr << "Can't load texture\n";
		return -1;
	}
	GLenum fmt;

	if (nrChannels == 4) {
		fmt = GL_RGBA;
	}
	else if (nrChannels == 3) {
		fmt = GL_RGB;
	}
	else {
		fmt = GL_RED;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);
	stbi_image_free(data);

	return texture;
}

class Model {
public:
	// model data
	Files* files;
	std::vector<Texture> textures_loaded; // stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	bool gammaCorrection;

	Model(Files* files, std::string const &fileName, bool gamma=false) 
		:files(files), gammaCorrection(gamma){
		loadModel(files->objectFile(fileName));
	}

	// draw the model, and thus its meshes
	void Draw(Shader* shader) {
		for (u32 i = 0; i < meshes.size(); ++i) {
			meshes[i].Draw(shader);
		}
	}

private:
	// load a model with supported ASSIMP extensions from file and stores the resulting meshes in meshes vector
	// load a model into ASSIMP data structured called 'Scene Object' -> access all the data
	void loadModel(std::string const& fileName) {
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(fileName, 
												  aiProcess_Triangulate
												| aiProcess_GenSmoothNormals
												| aiProcess_FlipUVs
												| aiProcess_CalcTangentSpace);

		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cerr << "Error::Assimp::" << importer.GetErrorString() << std::endl;
			return;
		}

		// proccess ASSIMP's root node recursively
		processNode(scene->mRootNode, scene);
	}

	// proccesses a node in a recursive fashion, process each individual mesh located at the node and repeats this process on it's children nodes (if any)
	void processNode(aiNode* node, const aiScene* scene) {

		// process all the node's meshes (if any)
		for (u32 i = 0; i < node->mNumMeshes; ++i) {
			// the node object only contains indices to index the actual objects in the scene
			// the scene contains all the data, node is just to keep stuff organized (like relations between nodes)
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		// then do the same for each of its children
		for (u32 i = 0; i < node->mNumChildren; ++i) {
			processNode(node->mChildren[i], scene);
		}
	}

	// Assimp to Mesh
	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		// data to fill
		std::vector<Vertex> vertices;
		std::vector<u32> indices;
		std::vector<Texture> textures;

		// proccess mesh's vertices
		for (u32 i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vertex;

			// process vertex positions
			vertex.Position = { mesh->mVertices[i].x,
								mesh->mVertices[i].y,
								mesh->mVertices[i].z };

			// process vertex normals
			if (mesh->HasNormals()) {
				vertex.Normal = { mesh->mNormals[i].x,
								mesh->mNormals[i].y,
								mesh->mNormals[i].z };
			}
			// process textures coordinates
			if (mesh->mTextureCoords[0]) { // Does the mesh contains texture coordinates?

				// a vertex can contain up to 8 different texture coordinate, so allways take the first set (0).
				vertex.TexCoords = { mesh->mTextureCoords[0][i].x,
									 mesh->mTextureCoords[0][i].y };
				vertex.Tangent = {  mesh->mTangents[i].x,
									mesh->mTangents[i].y,
									mesh->mTangents[i].z};
				vertex.Bitangent = { mesh->mBitangents[i].x,
									 mesh->mBitangents[i].y,
									 mesh->mBitangents[i].z };
			}
			else {
				vertex.TexCoords = glm::vec2(0.0f);
			}
			vertices.push_back(vertex);
		}

		// now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
		for (u32 i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (u32 j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		//process material
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// convention for sampler names in the shaders 
		// 
		// diffuse: texture_diffuseN
		// specular: texture_specularN
		// normal: texture_normalN

		std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material,
			aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		std::vector<Texture> heightMaps = loadMaterialTextures(material,
			aiTextureType_AMBIENT, "texture_height");
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		return Mesh(vertices, indices, textures);
	}

	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {
		std::vector<Texture> textures;

		for (u32 i = 0; i < mat->GetTextureCount(type); ++i) {
			aiString str;
			mat->GetTexture(type, i, &str);
			// check if the texture was loaded before and if so, continue to next iteration; skip loading a new texture
			bool skip = false;

			for (u32 j = 0; j < textures_loaded.size(); ++j) {
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				// if the texture hasn't been loaded already, load it
				Texture texture;
				std::string texFile = files->objectFile(std::string(str.C_Str()));
				texture.id = TextureFromFile(texFile);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); // store it as texture loaded for entire model to ensure we won't unnecesary load duplicated again
			}
		}
		return textures;
	}
};

#endif