#include <mesh.h>
Mesh makeSquare() {

	glm::vec3 vertices[] = {
		{0,0,0},//bottom left
		{1,0,0},//bottom right
		{1,1,0},//top right
		{0,1,0}//top left
	};
	int indices[] = {//counter clockwise indices
		0,1,2,
		0,2,3
	};
	//https://learnopengl.com/Getting-started/Hello-Triangle

	return Mesh(vertices, indices);
}

Mesh::Mesh(std::span<glm::vec3> vertices, std::span<int> indices)
{
	glGenVertexArrays(1, &vertexArrayObject);
	// ..:: Initialization code (done once (unless your object frequently changes)) :: ..
	// 1. bind Vertex Array Object
	glBindVertexArray(vertexArrayObject);

	glGenBuffers(1, &vertexBufferObject);
	// 2. copy our vertices array in a buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	//an index buffer
	glGenBuffers(1, &elementBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size_bytes(), indices.data(), GL_STATIC_DRAW);
	// 4. then set our vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}

void Mesh::render(int indiceCount)
{
	glBindVertexArray(vertexArrayObject);
	glDrawElements(GL_TRIANGLES, indiceCount, GL_UNSIGNED_INT, 0);
}
