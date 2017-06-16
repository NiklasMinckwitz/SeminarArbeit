#include "engine/graphics/directionallight.hpp"

#include <glm/gtx/transform.hpp>

DirectionalLight::DirectionalLight(Transform* transform) {
	this->transform = transform;
}

void DirectionalLight::use_shadow_buffer(Shader* shader) {
	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader->get_id());

	glm::mat4 view = get_light_view();

	glUniformMatrix4fv(shader->get_location("projection"), 1, GL_FALSE, &projection[0][0]);
	glUniformMatrix4fv(shader->get_location("view"), 1, GL_FALSE, &view[0][0]);
}

void DirectionalLight::set_cast_shadows(GLboolean value) {
	//TODO: white shadow map if cast_shadows = false
	if (value) {
		if (cast_shadows) return;
		glGenFramebuffers(1, &depth_frame_buffer);

		glGenTextures(1, &depth_map);
		glBindTexture(GL_TEXTURE_2D, depth_map);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_RESOLUTION, SHADOW_RESOLUTION, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		projection = glm::ortho(-3.5f, 3.5f, -3.5f, 3.5f, 1.0f, 10.0f);
	} else {
		if (!cast_shadows) return;
		glDeleteFramebuffers(1, &depth_frame_buffer);
		glDeleteTextures(1, &depth_map);
	}

	cast_shadows = value;
}

void DirectionalLight::set_color(glm::vec3 color) {
	this->color = color;
}

void DirectionalLight::set_intensity(GLfloat value) {
	intensity = value;
}

void DirectionalLight::set_bias(GLfloat value) {
	bias = value / SHADOW_RESOLUTION;
}

GLboolean DirectionalLight::get_cast_shadows() {
	return cast_shadows;
}

glm::mat4 DirectionalLight::get_light_view() {
	return glm::lookAt(-get_direction() * 7.5f, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

glm::mat4 DirectionalLight::get_light_projection() {
	return projection;
}

glm::vec3 DirectionalLight::get_color() {
	return color;
}

glm::vec3 DirectionalLight::get_direction() {
	return transform->get_forward();
}

GLfloat DirectionalLight::get_intensity() {
	return intensity;
}

GLfloat DirectionalLight::get_bias() {
	return bias;
}

GLuint DirectionalLight::get_shadow_map() {
	return depth_map;
}