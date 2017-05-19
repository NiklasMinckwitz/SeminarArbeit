#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "camera.hpp"

namespace engine {
	Camera* main_camera;

	void init();
	void clear();
	void render();
	void render_scene();
}

#endif