#pragma once
#include "safeInclude/includeGL.h"
struct Mesh {
	//just an array of floats
	GLuint vertexArrayObject;
	//a buffer, 'explaining' the float array
	GLuint vertexBufferObject;
	//an index buffer
	GLuint elementBufferObject;
};

Mesh makeSquare();