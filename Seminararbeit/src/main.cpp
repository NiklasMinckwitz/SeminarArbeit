#include "engine/engine.hpp"

#include <GLFW/glfw3.h>
#include <iostream>

#include "Terrain.hpp"

#define ESCAPE_BUTTON 1

#define UP_BUTTON 100
#define LEFT_BUTTON 101
#define RIGHT_BUTTON 102
#define DOWN_BUTTON 103

#define F1_BUTTON 104
#define F2_BUTTON 105
#define F3_BUTTON 106
#define F4_BUTTON 107
#define F5_BUTTON 108
#define F6_BUTTON 109

int main() {
	//init engines window, graphics, input
	engine::init();
	engine::set_resolution(1024, 768);
	
	engine::set_mouse_sensitivity(0.2f);

	engine::register_button(ESCAPE_BUTTON, GLFW_KEY_ESCAPE);

	engine::register_button(UP_BUTTON, GLFW_KEY_W);
	engine::register_button(UP_BUTTON, GLFW_KEY_UP);
	engine::register_button(LEFT_BUTTON, GLFW_KEY_A);
	engine::register_button(LEFT_BUTTON, GLFW_KEY_LEFT);
	engine::register_button(RIGHT_BUTTON, GLFW_KEY_D);
	engine::register_button(RIGHT_BUTTON, GLFW_KEY_RIGHT);
	engine::register_button(DOWN_BUTTON, GLFW_KEY_S);
	engine::register_button(DOWN_BUTTON, GLFW_KEY_DOWN);

	engine::register_button(F1_BUTTON, GLFW_KEY_F1);
	engine::register_button(F2_BUTTON, GLFW_KEY_F2);
	engine::register_button(F3_BUTTON, GLFW_KEY_F3);
	engine::register_button(F4_BUTTON, GLFW_KEY_F4);
	engine::register_button(F5_BUTTON, GLFW_KEY_F5);
	engine::register_button(F6_BUTTON, GLFW_KEY_F6);

	//load resources
	Shader* geometry_shader = engine::load_shader("res/shaders/geometry.shader");

	Texture* octostone_albedo = engine::load_texture("res/textures/octostone/albedo.png");
	Texture* octostone_metallic = engine::load_texture("res/textures/octostone/metallic.png");
	Texture* octostone_roughness = engine::load_texture("res/textures/octostone/roughness.png");
	Texture* octostone_normal = engine::load_texture("res/textures/octostone/normal.png");

	Texture* rubber_albedo = engine::load_texture("res/textures/rubber/albedo512.png");
	Texture* rubber_metallic = engine::load_texture("res/textures/rubber/metallic512.png");
	Texture* rubber_roughness = engine::load_texture("res/textures/rubber/roughness512.png");
	Texture* rubber_normal = engine::load_texture("res/textures/rubber/normal512.png");

	//TODO: tile iron texture
	Texture* iron_albedo = engine::load_texture("res/textures/iron/albedo512.png");
	Texture* iron_metallic = engine::load_texture("res/textures/iron/metallic512.png");
	Texture* iron_roughness = engine::load_texture("res/textures/iron/roughness512.png");
	Texture* iron_normal = engine::load_texture("res/textures/iron/normal512.png");

	Material* mat_octostone = engine::create_material(geometry_shader);
	mat_octostone->set_vec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
	mat_octostone->set_texture("albedo", octostone_albedo);
	mat_octostone->set_texture("metallic", octostone_metallic);
	mat_octostone->set_texture("roughness", octostone_roughness);
	mat_octostone->set_texture("normal_map", octostone_normal);
	mat_octostone->set_vec2("tiling", glm::vec2(1.0f, 1.0f));

	Material* mat_rubber = engine::create_material(geometry_shader);
	mat_rubber->set_vec3("color", glm::vec3(0.1f, 0.3f, 0.1f));
	mat_rubber->set_texture("albedo", rubber_albedo);
	mat_rubber->set_texture("metallic", rubber_metallic);
	mat_rubber->set_texture("roughness", rubber_roughness);
	mat_rubber->set_texture("normal_map", rubber_normal);
	mat_rubber->set_vec2("tiling", glm::vec2(1.0f, 1.0f));

	Material* mat_iron = engine::create_material(geometry_shader);
	mat_iron->set_vec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
	mat_iron->set_texture("albedo", iron_albedo);
	mat_iron->set_texture("metallic", iron_metallic);
	mat_iron->set_texture("roughness", iron_roughness);
	mat_iron->set_texture("normal_map", iron_normal);
	mat_iron->set_vec2("tiling", glm::vec2(15.0f, 15.0f));

	Mesh* container_mesh = engine::load_mesh("res/models/container1.obj");

	//create scene
	GameObject* camera1 = engine::create_gameobject();
	{
		Camera* camera_comp = camera1->add_camera_component();
		camera_comp->set_aspect_ratio(1024.0f / 768.0f);
		camera_comp->set_exposure(1.0f);
		camera1->transform.set_position(glm::vec3(0.0f, 0.25f, 3.0f));
	}

	GameObject* dir_light1 = engine::create_gameobject();
	{
		DirectionalLight* dir_light = dir_light1->add_directionallight_component();
		dir_light1->transform.set_rotation(glm::quat(glm::vec3(glm::radians(50.0f), glm::radians(-30.0f), 0)));

		dir_light->set_intensity(10.0f);
		dir_light->set_color(glm::vec3(1.0f, 0.8f, 0.6f));
		dir_light->set_cast_shadows(GL_TRUE);
		dir_light->set_bias(5.0f);
	}

	/*GameObject* dir_light2 = engine::create_gameobject();
	{
		DirectionalLight* dir_light = dir_light2->add_directionallight_component();
		dir_light2->transform.set_rotation(glm::quat(glm::vec3(glm::radians(50.0f), glm::radians(30.0f), 0)));

		dir_light->set_intensity(5.0f);
		dir_light->set_color(glm::vec3(1.0f, 0.8f, 0.6f));
		dir_light->set_cast_shadows(GL_TRUE);
	}

	GameObject* dir_light3 = engine::create_gameobject();
	{
		DirectionalLight* dir_light = dir_light3->add_directionallight_component();
		dir_light3->transform.set_rotation(glm::quat(glm::vec3(glm::radians(-50.0f), glm::radians(180.0f), 0)));

		dir_light->set_intensity(5.0f);
		dir_light->set_color(glm::vec3(1.0f, 0.8f, 0.6f));
		dir_light->set_cast_shadows(GL_TRUE);
	}*/

	Terrain terrain(mat_rubber, 10, 10);
	terrain.game_object->transform.set_position(glm::vec3(0, -1.0f, 0));
	terrain.game_object->transform.set_scale(glm::vec3(1.25f, 1.0f, 1.25f));

	GameObject* test_quad = engine::create_gameobject();
	{
		MeshRenderer* mesh_renderer = test_quad->add_meshrenderer_component();
		mesh_renderer->material = mat_iron;
		mesh_renderer->mesh = container_mesh;
		test_quad->transform.set_position(glm::vec3(0, -1.5f, 0));
		test_quad->transform.set_rotation(glm::quat(glm::vec3(0, 0, 0)));
		test_quad->transform.set_scale(glm::vec3(2, 2, 2));
	}

	//variables
	const GLfloat camera_speed = 0.1f;
	GLfloat yaw = 0, pitch = 0;

	//main loop
	while (!engine::should_close()) {
		engine::update();

		if (engine::button_hit(ESCAPE_BUTTON)) {
			engine::close();
		}

		glm::vec2 mouse_off = engine::get_mouse_offset();
		pitch += mouse_off.y;
		yaw += mouse_off.x;
		pitch = glm::clamp(pitch, -85.0f, 85.0f);

		glm::quat q_pitch = glm::angleAxis(glm::radians(pitch), glm::vec3(1, 0, 0));
		glm::quat q_yaw = glm::angleAxis(glm::radians(yaw), glm::vec3(0, 1, 0));
		camera1->transform.set_rotation(glm::normalize(q_pitch * q_yaw));

		glm::quat g_rotation_x = glm::angleAxis(glm::radians(30.0f) * glm::radians(45.0f + 45.0f * glm::sin(0.1f * (GLfloat)glfwGetTime())), glm::vec3(1, 0, 0));
		glm::quat q_rotation_y = glm::angleAxis(glm::radians((GLfloat)glfwGetTime()) * 10.0f, glm::vec3(0, 1, 0));
		dir_light1->transform.set_rotation(glm::normalize(g_rotation_x * q_rotation_y));

		if (engine::button_down(UP_BUTTON)) {
			camera1->transform.translate(camera1->transform.get_forward() * camera_speed);
		}
		else if (engine::button_down(DOWN_BUTTON)) {
			camera1->transform.translate(-camera1->transform.get_forward() * camera_speed);
		}

		if (engine::button_down(LEFT_BUTTON)) {
			camera1->transform.translate(-camera1->transform.get_right() * camera_speed);
		}
		else if (engine::button_down(RIGHT_BUTTON)) {
			camera1->transform.translate(camera1->transform.get_right() * camera_speed);
		}

		if (engine::button_down(F1_BUTTON)) {
			engine::set_render_mode(0);
		} else if (engine::button_down(F2_BUTTON)) {
			engine::set_render_mode(1);
		} else if (engine::button_down(F3_BUTTON)) {
			engine::set_render_mode(2);
		} else if (engine::button_down(F4_BUTTON)) {
			engine::set_render_mode(3);
		} else if (engine::button_down(F5_BUTTON)) {
			engine::set_render_mode(4);
		} else if (engine::button_down(F6_BUTTON)) {
			engine::set_render_mode(5);
		}

		engine::late_update();
		engine::render();
	}

	//clear all memory allocations
	engine::clear();
}