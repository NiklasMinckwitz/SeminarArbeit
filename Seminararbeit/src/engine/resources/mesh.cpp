#include "engine/resources/mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>

Mesh::Mesh() {

}

Mesh::Mesh(std::string path) {
	load(path);
}

void Mesh::load(std::string path) {
	//initialize assimp importer
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "Assimp Importer Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	//load mesh vertices, normals and uvs with assimp
	aiMesh* aimesh = scene->mMeshes[0];
	for (unsigned int i = 0; i < aimesh->mNumVertices; ++i) {
		glm::vec3 vertex;
		glm::vec3 normal;
		glm::vec2 uv;
		glm::vec3 tangent;

		vertex.x = aimesh->mVertices[i].x;
		vertex.y = aimesh->mVertices[i].y;
		vertex.z = aimesh->mVertices[i].z;

		normal.x = aimesh->mNormals[i].x;
		normal.y = aimesh->mNormals[i].y;
		normal.z = aimesh->mNormals[i].z;

		tangent.x = aimesh->mTangents[i].x;
		tangent.y = aimesh->mTangents[i].y;
		tangent.z = aimesh->mTangents[i].z;

		if (aimesh->mTextureCoords[0]) {
			uv.x = aimesh->mTextureCoords[0][i].x;
			uv.y = aimesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
		normals.push_back(normal);
		uvs.push_back(uv);
		tangents.push_back(tangent);
	}

	//load mesh indices
	for (unsigned int i = 0; i < aimesh->mNumFaces; ++i) {
		aiFace face = aimesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j) {
			indices.push_back(face.mIndices[j]);
		}
	}

	//apply vertices, normals, uvs and indices
	apply();
}

void Mesh::clear() {
	vertices.clear();
	indices.clear();
	normals.clear();
	uvs.clear();
	tangents.clear();

	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);
	glDeleteBuffers(1, &nbo);
	glDeleteBuffers(1, &uvbo);
	glDeleteBuffers(1, &tbo);
	glDeleteVertexArrays(1, &vao);
}

void Mesh::calculate_normals() {
	normals.clear();
	for (GLuint i = 0; i < vertices.size(); ++i) {
		normals.push_back(glm::vec3(0, 0, 0));
	}

	for (GLuint i = 0; i < indices.size(); i += 3) {
		glm::vec3 u = vertices[indices[i + 1]] - vertices[indices[i]];
		glm::vec3 v = vertices[indices[i + 2]] - vertices[indices[i]];

		glm::vec3 normal = glm::normalize(glm::vec3(
			u.y * v.z - u.z * v.y,
			u.z * v.x - u.x * v.z,
			u.x * v.y - u.y * v.x
		));

		normals[indices[i]] += normal;
		normals[indices[i + 1]] += normal;
		normals[indices[i + 2]] += normal;
	}

	for (GLuint i = 0; i < normals.size(); ++i) {
		normals[i] = glm::normalize(normals[i]);
	}
}

void Mesh::calculate_tangents() {
	tangents.clear();
	for (GLuint i = 0; i < vertices.size(); ++i) {
		tangents.push_back(glm::vec3(0, 0, 0));
	}

	for (GLuint i = 0; i < indices.size(); i += 3) {
		glm::vec3 edge1 = vertices[indices[i + 1]] - vertices[indices[i]];
		glm::vec3 edge2 = vertices[indices[i + 2]] - vertices[indices[i]];
		glm::vec2 delta_uv1 = uvs[indices[i + 1]] - uvs[indices[i]];
		glm::vec2 delta_uv2 = uvs[indices[i + 2]] - uvs[indices[i]];

		GLfloat f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);

		glm::vec3 tangent = glm::normalize(glm::vec3(
			f * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x),
			f * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y),
			f * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z)
		));

		tangents[indices[i]] = tangent;
		tangents[indices[i + 1]] = tangent;
		tangents[indices[i + 2]] = tangent;
	}

	for (GLuint i = 0; i < tangents.size(); ++i) {
		tangents[i] = glm::normalize(tangents[i]);
	}
}

void Mesh::apply() {
	//generate vao and bind
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//generate vbo and bind to vao
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(GLfloat), &vertices.data()[0], GL_STATIC_DRAW);

	//generate ibo and bind to vao
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	//generate nbo and bind to vao
	glGenBuffers(1, &nbo);
	glBindBuffer(GL_ARRAY_BUFFER, nbo);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * 3 * sizeof(GLfloat), &normals.data()[0], GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	//generate uvbo and bind to vao
	glGenBuffers(1, &uvbo);
	glBindBuffer(GL_ARRAY_BUFFER, uvbo);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * 2 * sizeof(GLfloat), &uvs.data()[0], GL_STATIC_DRAW);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(2);

	//generate tbo and bind to vao
	glGenBuffers(1, &tbo);
	glBindBuffer(GL_ARRAY_BUFFER, tbo);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * 3 * sizeof(GLfloat), &tangents.data()[0], GL_STATIC_DRAW);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(3);

	//unbind vao
	glBindVertexArray(0);
}