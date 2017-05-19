#include "graphics/graphics.hpp"

#include "graphics/material.hpp"
#include "graphics/shader.hpp"
#include "geometry/mesh.hpp"

namespace engine {
	Shader* firstpass_shader;
	Shader* secondpass_shader;
	Shader* depth_shader;

	Mesh* frame_quad;
	Mesh* terrain;
	Mesh* character;

	void load_frame_quad();

	void init() {
		secondpass_shader = new Shader("res/shaders/secondpass.shader");
		depth_shader = new Shader("res/shaders/depth.shader");

		load_frame_quad();
	}

	void clear() {
		secondpass_shader->clear();
		depth_shader->clear();
		delete secondpass_shader;
		delete depth_shader;

		frame_quad->clear();
		terrain->clear();
		character->clear();
		delete frame_quad;
		delete terrain;
		delete character;
	}

	void render() {
		//firstpass
		render_scene();

		//secondpass
	}

	void render_scene() {

	}

	void load_frame_quad() {
		std::vector<glm::vec3> vertices;
		std::vector<GLuint> indices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

		vertices.push_back(glm::vec3(-1.0f, -1.0f, 0));
		vertices.push_back(glm::vec3(1.0f, -1.0f, 0));
		vertices.push_back(glm::vec3(-1.0f, 1.0f, 0));
		vertices.push_back(glm::vec3(1.0f, 1.0f, 0));

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);

		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));
		normals.push_back(glm::vec3(0, 0, 1));

		uvs.push_back(glm::vec2(0, 0));
		uvs.push_back(glm::vec2(1, 0));
		uvs.push_back(glm::vec2(0, 1));
		uvs.push_back(glm::vec2(1, 1));

		frame_quad = new Mesh();
		frame_quad->vertices = vertices;
		frame_quad->indices = indices;
		frame_quad->normals = normals;
		frame_quad->uvs = uvs;
		frame_quad->apply();
	}
}