#pragma once
#include "safeInclude/includeGL.h"
#include <span>
#include <glm/vec3.hpp>
struct Mesh {
	//just an array of floats
	GLuint vertexArrayObject;
	//a buffer, 'explaining' the float array
	GLuint vertexBufferObject;
	//an index buffer
	GLuint elementBufferObject;
	Mesh() = default;
	Mesh(std::span<glm::vec3> vertices, std::span<int> indices);
	void render(int indiceCount);
};

Mesh makeSquare();