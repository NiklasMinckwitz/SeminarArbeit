#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include "texture.hpp"

struct Material {
	Texture* albedo;
	Texture* roughness;
	Texture* metallic;
	Texture* normal_map;
};

#endif