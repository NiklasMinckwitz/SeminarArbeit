#include "Terrain.hpp"

#include <glm/gtc/noise.hpp>

#include <iostream>

Terrain::Terrain(Material* material, GLuint width, GLuint height) {
	game_object = engine::create_gameobject();

	this->material = material;
	set_size(width, height);
}

void Terrain::generate_terrain() {
	seed = 5;

	terrain_data = new GLfloat[width * height];
	for (GLuint ix = 0; ix < width; ++ix) {
		for (GLuint iz = 0; iz < height; ++iz) {
			GLuint i = ix * height + iz;
			//terrain_data[i] = 0.25f * glm::abs(glm::sin(0.1f * i) + glm::cos(0.1f * i));// glm::perlin(glm::vec4(i, i, seed, seed));
			terrain_data[i] = 0.5f * glm::simplex(glm::vec2(0.05f * ix, 0.05f * iz));
			terrain_data[i] = 0.5f * glm::simplex(glm::vec2(ix, iz));
		}
	}
}

void Terrain::set_size(GLuint width, GLuint height) {
	this->width = width;
	this->height = height;

	generate_terrain();

	if(tiles != nullptr) clear();
	tiles = new GameObject*[(width - 1) * (height - 1)];
	for (GLuint ix = 0; ix < width - 1; ++ix) {
		for (GLuint iz = 0; iz < height - 1; ++iz) {
			GLuint i = ix * (height - 1) + iz;
			tiles[i] = engine::create_gameobject();
			{
				MeshRenderer* mesh_renderer = tiles[i]->add_meshrenderer_component();
				mesh_renderer->mesh = nullptr;
				mesh_renderer->material = material;
				tiles[i]->transform.set_parent(&game_object->transform);
				tiles[i]->transform.set_scale(glm::vec3(3.0f / width, 0.5f, 3.0f / height));
			}
		}
	}
	calculate_normals();
	update_view();
}

void Terrain::scroll_view(GLfloat x, GLfloat z) {
	this->x = x;
	this->z = z;
	update_view();
}

void Terrain::clear() {
	for (GLuint ix = 0; ix < width - 1; ++ix) {
		for (GLuint iz = 0; iz < height - 1; ++iz) {
			GLuint i = ix * (height - 1) + iz;
			if (tiles[i] != nullptr) {
				MeshRenderer* mesh_renderer = tiles[i]->get_meshrenderer_component();
				if (mesh_renderer != nullptr) {
					if (mesh_renderer->mesh != nullptr) {
						delete mesh_renderer->mesh;
						mesh_renderer->mesh = nullptr;
					}
				}
			}
		}
	}
}

void Terrain::calculate_normals() {
	terrain_normals = new glm::vec3[width * height];

	for (int i = 0; i < width * height; ++i) {
		terrain_normals[i] = glm::vec3(0, 0, 0);
	}

	for (GLuint ix = 0; ix < width - 1; ++ix) {
		for (GLuint iz = 0; iz < height - 1; ++iz) {
			GLuint i = ix * (height - 1) + iz;

			glm::vec3 v1 = glm::vec3(-0.5f, 0.5f + terrain_data[i], -0.5f);
			glm::vec3 v2 = glm::vec3(-0.5f, 0.5f + terrain_data[i + 1], 0.5f);
			glm::vec3 v3 = glm::vec3(0.5f, 0.5f + terrain_data[i + width - 1], -0.5f);
			glm::vec3 v4 = glm::vec3(0.5f, 0.5f + terrain_data[i + width], 0.5f);

			glm::vec3 u = v2 - v1;
			glm::vec3 v = v3 - v1;
			glm::vec3 normal1 = glm::normalize(glm::vec3(
				u.y * v.z - u.z * v.y,
				u.z * v.x - u.x * v.z,
				u.x * v.y - u.y * v.x
			));
			u = v2 - v3;
			v = v4 - v3;
			glm::vec3 normal2 = glm::normalize(glm::vec3(
				u.y * v.z - u.z * v.y,
				u.z * v.x - u.x * v.z,
				u.x * v.y - u.y * v.x
			));
			glm::vec3 normal3 = glm::normalize(0.5f * (normal1 + normal2));

			terrain_normals[i] = glm::normalize(0.5f * (terrain_normals[i] + normal1));
			terrain_normals[i + 1] = glm::normalize(0.5f * (terrain_normals[i] + normal3));
			terrain_normals[i + width - 1] = glm::normalize(0.5f * (terrain_normals[i] + normal3));
			terrain_normals[i + width] = glm::normalize(0.5f * (terrain_normals[i] + normal2));
		}
	}
}

void Terrain::update_view() {
	GLfloat off_x, off_z;
	
	off_x = glm::mod(x, (3.0f / width));
	off_z = glm::mod(z, (3.0f / height));

	for (GLuint ix = 0; ix < width - 1; ++ix) {
		for (GLuint iz = 0; iz < height - 1; ++iz) {
			GLuint i = ix * (height - 1) + iz;
			MeshRenderer* mesh_renderer = tiles[i]->get_meshrenderer_component();
			if (mesh_renderer->mesh == nullptr) {
				mesh_renderer->mesh = generate_cube(i);
			}
			tiles[i]->transform.set_position(glm::vec3(-0.5f * 3.0f + (ix + 1) * (3.0f / width) + off_x, 0, -0.5f * 3.0f + (iz + 1) * (3.0f / height) + off_z));
		}
	}
}

Mesh* Terrain::generate_cube(GLuint i) {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<GLuint> indices;
	std::vector<glm::vec2> uvs;

	float v1 = terrain_data[i];
	float v2 = terrain_data[i + 1];
	float v3 = terrain_data[i + width - 1];
	float v4 = terrain_data[i + width];

	//front
	vertices.push_back(glm::vec3(-0.5f, 0, 0.5f));
	vertices.push_back(glm::vec3(0.5f, 0, 0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0.5f + v2, 0.5f));
	vertices.push_back(glm::vec3(0.5f, 0.5f + v4, 0.5f));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	normals.push_back(glm::vec3(0, 0, 1));
	//back
	vertices.push_back(glm::vec3(-0.5f, 0, -0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0.5f + v1, -0.5f));
	vertices.push_back(glm::vec3(0.5f, 0, -0.5f));
	vertices.push_back(glm::vec3(0.5f, 0.5f + v3, -0.5f));
	normals.push_back(glm::vec3(0, 0, -1));
	normals.push_back(glm::vec3(0, 0, -1));
	normals.push_back(glm::vec3(0, 0, -1));
	normals.push_back(glm::vec3(0, 0, -1));
	//left
	vertices.push_back(glm::vec3(-0.5f, 0, -0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0, 0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0.5f + v1, -0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0.5f + v2, 0.5f));
	normals.push_back(glm::vec3(-1, 0, 0));
	normals.push_back(glm::vec3(-1, 0, 0));
	normals.push_back(glm::vec3(-1, 0, 0));
	normals.push_back(glm::vec3(-1, 0, 0));
	//right
	vertices.push_back(glm::vec3(0.5f, 0, -0.5f));
	vertices.push_back(glm::vec3(0.5f, 0.5f + v3, -0.5f));
	vertices.push_back(glm::vec3(0.5f, 0, 0.5f));
	vertices.push_back(glm::vec3(0.5f, 0.5f + v4, 0.5f));
	normals.push_back(glm::vec3(1, 0, 0));
	normals.push_back(glm::vec3(1, 0, 0));
	normals.push_back(glm::vec3(1, 0, 0));
	normals.push_back(glm::vec3(1, 0, 0));
	//top
	vertices.push_back(glm::vec3(-0.5f, 0.5f + v1, -0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0.5f + v2, 0.5f));
	vertices.push_back(glm::vec3(0.5f, 0.5f + v3, -0.5f));
	vertices.push_back(glm::vec3(0.5f, 0.5f + v4, 0.5f));
	normals.push_back(terrain_normals[i]);
	normals.push_back(terrain_normals[i + 1]);
	normals.push_back(terrain_normals[i + width - 1]);
	normals.push_back(terrain_normals[i + width]);
	//bottom
	vertices.push_back(glm::vec3(-0.5f, 0, -0.5f));
	vertices.push_back(glm::vec3(0.5f, 0, -0.5f));
	vertices.push_back(glm::vec3(-0.5f, 0, 0.5f));
	vertices.push_back(glm::vec3(0.5f, 0, 0.5f));
	normals.push_back(glm::vec3(0, -1, 0));
	normals.push_back(glm::vec3(0, -1, 0));
	normals.push_back(glm::vec3(0, -1, 0));
	normals.push_back(glm::vec3(0, -1, 0));

	for (int i = 0; i < 6; ++i) {
		uvs.push_back(glm::vec2(0, 0));
		uvs.push_back(glm::vec2(1, 0));
		uvs.push_back(glm::vec2(0, 1));
		uvs.push_back(glm::vec2(1, 1));

		indices.push_back(i * 4 + 0);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 2);
		indices.push_back(i * 4 + 1);
		indices.push_back(i * 4 + 3);
	}

	Mesh* mesh = new Mesh();
	mesh->vertices = vertices;
	mesh->normals = normals;
	mesh->indices = indices;
	mesh->uvs = uvs;

	//mesh->calculate_normals();
	mesh->calculate_tangents();
	mesh->apply();
	return mesh;
}