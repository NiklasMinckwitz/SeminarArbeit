#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "engine/engine.hpp"

class Terrain {
public:
	Terrain(Material* material, GLuint width, GLuint height);

	void generate_terrain();
	void set_size(GLuint width, GLuint height);
	void scroll_view(GLfloat x, GLfloat y);
	
	void clear();

	GameObject* game_object;
private:
	Material* material;
	GLuint width, height;
	GLfloat x, z;

	GLuint seed;
	GLfloat* terrain_data;
	glm::vec3* terrain_normals;
	GameObject** tiles;

	void calculate_normals();
	void update_view();
	Mesh* generate_cube(GLuint i);
};

#endif