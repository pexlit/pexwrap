#pragma once
#include "safeInclude/includeGL.h"
#include <vector>
#include <filesystem>
struct Texture {
	GLuint width{}, height{}, ID{};
	uint8_t* data{};

	Texture(GLuint width, GLuint height, uint8_t* data = nullptr);
	Texture(const std::filesystem::path& path);
	~Texture();
	void setPixel(GLuint x, GLuint y, uint32_t color);
private:
	void toGL();
};