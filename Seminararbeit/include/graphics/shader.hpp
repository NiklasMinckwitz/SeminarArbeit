#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
	Shader(std::string path);

	GLuint get_id();
	void clear();

	void use();

	void set_uniform(std::string name, GLuint value, GLboolean use_shader = GL_FALSE);
	void set_uniform(std::string name, GLint value, GLboolean use_shader = GL_FALSE);
	void set_uniform(std::string name, GLfloat value, GLboolean use_shader = GL_FALSE);
	void set_uniform(std::string name, glm::vec2 value, GLboolean use_shader = GL_FALSE);
	void set_uniform(std::string name, glm::vec3 value, GLboolean use_shader = GL_FALSE);
	void set_uniform(std::string name, glm::vec4 value, GLboolean use_shader = GL_FALSE);
	void set_uniform(std::string name, glm::mat3 value, GLboolean use_shader = GL_FALSE);
	void set_uniform(std::string name, glm::mat4 value, GLboolean use_shader = GL_FALSE);
private:
	GLuint program_id;

	std::unordered_map<std::string, GLuint> locations;
};

#endif