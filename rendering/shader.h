#pragma once
#include "safeInclude/includeGL.h"
#include <string>
#include <filesystem>
//the type of a shader (fragment or vertex)
enum class ShaderType {
	Fragment = GL_FRAGMENT_SHADER,
	Vertex = GL_VERTEX_SHADER
};
struct Shader {
	Shader(const std::filesystem::path& path);
	GLuint ID;
	//GL_FRAGMENT_SHADER or GL_VERTEX_SHADER
	ShaderType type;
	//wether the shader compiled correctly
	bool success;
};