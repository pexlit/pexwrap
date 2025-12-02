#include <mesh.h>
Mesh makeSquare() {

	float vertices[] = {
		0,0,0,//bottom left
		1,0,0,//bottom right
		1,1,0,//top right
		0,1,0//top left
	};
	int indices[] = {//counter clockwise indices
		0,1,2,
		0,2,3
	};
	//https://learnopengl.com/Getting-started/Hello-Triangle

	Mesh mesh;
	glGenVertexArrays(1, &mesh.vertexArrayObject);
	// ..:: Initialization code (done once (unless your object frequently changes)) :: ..
	// 1. bind Vertex Array Object
	glBindVertexArray(mesh.vertexArrayObject);

	glGenBuffers(1, &mesh.vertexBufferObject);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	//an index buffer
	glGenBuffers(1, &mesh.elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	// 4. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	return mesh;
}