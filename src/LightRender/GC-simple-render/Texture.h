#pragma once
#include <Render/Texture.h>

struct Texture : gce::Texture
{
	Texture(std::string_view const path) : gce::Texture(path, gce::TextureType::ALBEDO) {}

	friend class Geometry;
};