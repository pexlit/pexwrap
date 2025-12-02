#pragma once
#include "shader.h"
#include <Uniform.h>
#include <vector>
struct ShaderProgram {
	GLuint ID;
	Shader vertexShader;
	Shader fragmentShader;
	std::vector<Uniform> uniforms;
	ShaderProgram(Shader vertexShader, Shader fragmentShader): vertexShader(vertexShader), fragmentShader(fragmentShader) {
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader.ID);
		glAttachShader(ID, fragmentShader.ID);
		glLinkProgram(ID);

		GLint count;
		glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &count);

		const GLsizei bufSize = 16; // maximum name length
		GLchar nameBuffer[bufSize]; // variable name in GLSL
		GLsizei nameLength; // name length

		for (int i = 0; i < count; i++)
		{
			Uniform uniform{
				.location = (GLuint)i,
				.shaderProgramID = ID
			};
			glGetActiveUniform(ID, uniform.location, bufSize, &nameLength, &uniform.size, &uniform.type, nameBuffer);
			uniform.name = std::string(nameBuffer, nameLength);
			uniforms.push_back(uniform);
		}
	}
	void enable() {
		glUseProgram(ID);
	}
	Uniform* getUniform(std::string name) {
		GLint location = glGetUniformLocation(ID, name.c_str());
		if (location != -1) {
			return &uniforms[location];
		}
		else {
			return nullptr;
		}
	}
};