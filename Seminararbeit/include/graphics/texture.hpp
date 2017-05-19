#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <GL/glew.h>
#include <string>

class Texture {
public:
	Texture(GLuint width, GLuint height, GLint format = GL_RGB, GLboolean mip_mapping = GL_TRUE, GLint min_filter = GL_LINEAR_MIPMAP_LINEAR, GLint mag_filter = GL_LINEAR);
	Texture(std::string path, GLboolean mip_mapping = GL_TRUE, GLint min_filter = GL_LINEAR_MIPMAP_LINEAR, GLint mag_filter = GL_LINEAR);

	void clear();
private:
	GLuint texture_id;
};

#endif