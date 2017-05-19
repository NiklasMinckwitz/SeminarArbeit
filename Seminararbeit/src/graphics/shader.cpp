#include "graphics/shader.hpp"

#include <fstream>
#include <iostream>

Shader::Shader(std::string path) {
	//check if shader exists
	std::ifstream shader_stream(path);
	if (!shader_stream.is_open()) {
		std::cout << "Shader (" + path + ") could not be found." << std::endl;
		return;
	}

	//read shader
	std::string geometry_shader_code;
	std::string vertex_shader_code;
	std::string fragment_shader_code;

	std::string line = "";
	std::string* current_code = nullptr;

	while (std::getline(shader_stream, line)) {
		if (line == "#GEOMETRY SHADER") {
			current_code = &geometry_shader_code;
		} else if (line == "#VERTEX SHADER") {
			current_code = &vertex_shader_code;
		} else if (line == "#FRAGMENT SHADER") {
			current_code = &fragment_shader_code;
		} else if (line == "#GEOMETRY SHADER END" || line == "#VERTEX SHADER END" || line == "#FRAGMENT SHADER END") {
			current_code = nullptr;
		} else {
			if (current_code != nullptr) {
				(*current_code) += line + "\n";
			}
		}
	}
	shader_stream.close();

	//compile shader
	program_id = glCreateProgram();

	std::cout << "compiling shader program: " + path << std::endl;
	GLuint geometry_shader_id;
	GLuint vertex_shader_id;
	GLuint fragment_shader_id;

	GLint result = GL_FALSE;
	GLint info_log_length;

	//compile geometry shader
	if (!geometry_shader_code.empty()) {
		geometry_shader_id = glCreateShader(GL_GEOMETRY_SHADER);
		const GLchar *geometry_shader_code_c = geometry_shader_code.c_str();
		glShaderSource(geometry_shader_id, 1, &geometry_shader_code_c, NULL);
		glCompileShader(geometry_shader_id);
		glGetShaderiv(geometry_shader_id, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderiv(geometry_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
			GLchar* info_log = new GLchar[info_log_length];
			glGetShaderInfoLog(geometry_shader_id, info_log_length, NULL, info_log);
			std::cout << "Geometry shader compile error: " << info_log << std::endl;
		}
		glAttachShader(program_id, geometry_shader_id);
	}

	//compile vertex shader
	if (!vertex_shader_code.empty()) {
		vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
		const GLchar *vertex_shader_code_c = vertex_shader_code.c_str();
		glShaderSource(vertex_shader_id, 1, &vertex_shader_code_c, NULL);
		glCompileShader(vertex_shader_id);
		glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
			GLchar* info_log = new GLchar[info_log_length];
			glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, info_log);
			std::cout << "Geometry shader compile error: " << info_log << std::endl;
			delete[] info_log;
		}
		glAttachShader(program_id, vertex_shader_id);
	}

	//compile fragment shader
	if (!fragment_shader_code.empty()) {
		fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
		const GLchar *fragment_shader_code_c = fragment_shader_code.c_str();
		glShaderSource(fragment_shader_id, 1, &fragment_shader_code_c, NULL);
		glCompileShader(fragment_shader_id);
		glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
			GLchar* info_log = new GLchar[info_log_length];
			glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, info_log);
			std::cout << "Geometry shader compile error: " << info_log << std::endl;
			delete[] info_log;
		}
		glAttachShader(program_id, fragment_shader_id);
	}
	
	glLinkProgram(program_id);
	glGetProgramiv(program_id, GL_LINK_STATUS, &result);
	if (!result) {
		glGetShaderiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
		GLchar* info_log = new GLchar[info_log_length];
		glGetShaderInfoLog(program_id, info_log_length, NULL, info_log);
		std::cout << "Shader program compile error: " << info_log << std::endl;
		delete[] info_log;
	}

	//find uniform locations
	GLint uniform_count;
	glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &uniform_count);
	GLint max_name_length = 128;
	GLint size;
	GLenum type;
	GLsizei length;
	GLchar *name = new GLchar[max_name_length];

	for (GLuint i = 0; i < uniform_count; ++i) {
		glGetActiveUniform(program_id, i, max_name_length, &length, &size, &type, name);
		locations[name] = i;
	}
	delete[] name;

	if (!geometry_shader_code.empty()) {
		glDetachShader(program_id, geometry_shader_id);
		glDeleteShader(geometry_shader_id);
	}
	if (!vertex_shader_code.empty()) {
		glDetachShader(program_id, vertex_shader_id);
		glDeleteShader(vertex_shader_id);
	}
	if (!fragment_shader_code.empty()) {
		glDetachShader(program_id, fragment_shader_id);
		glDeleteShader(fragment_shader_id);
	}
}

GLuint Shader::get_id() {
	return program_id;
}

void Shader::clear() {
	glDeleteProgram(program_id);
}

void Shader::use() {
	glUseProgram(program_id);
}

void Shader::set_uniform(std::string name, GLuint value, GLboolean use_shader) {
	if (use_shader) use();
	glUniform1ui(locations[name], value);
}

void Shader::set_uniform(std::string name, GLint value, GLboolean use_shader) {
	if (use_shader) use();
	glUniform1i(locations[name], value);
}

void Shader::set_uniform(std::string name, GLfloat value, GLboolean use_shader) {
	if (use_shader) use();
	glUniform1f(locations[name], value);
}

void Shader::set_uniform(std::string name, glm::vec2 value, GLboolean use_shader) {
	if (use_shader) use();
	glUniform2f(locations[name], value.x, value.y);
}

void Shader::set_uniform(std::string name, glm::vec3 value, GLboolean use_shader) {
	if (use_shader) use();
	glUniform3f(locations[name], value.x, value.y, value.z);
}

void Shader::set_uniform(std::string name, glm::vec4 value, GLboolean use_shader) {
	if (use_shader) use();
	glUniform4f(locations[name], value.x, value.y, value.z, value.w);
}

void Shader::set_uniform(std::string name, glm::mat3 value, GLboolean use_shader) {
	if (use_shader) use();
	glUniformMatrix3fv(locations[name], 1, GL_FALSE, &value[0][0]);
}

void Shader::set_uniform(std::string name, glm::mat4 value, GLboolean use_shader) {
	if (use_shader) use();
	glUniformMatrix4fv(locations[name], 1, GL_FALSE, &value[0][0]);
}