#pragma once
#include "safeInclude/includeGL.h"
#include <glm/vec4.hpp>
#include <type_traits>
#include <glm/mat4x4.hpp>
struct Uniform {
	//location in the shader
	GLuint location;
	GLuint shaderProgramID;
	GLint size;
	std::string name;
	//float, vec3 or mat4, etc
	GLenum type;
	//warning! switches program!
	template<typename ValueType>
	void setValue(const ValueType& newValue) {
		glUseProgram(shaderProgramID);
		if constexpr (std::is_same_v< ValueType, glm::vec4>) {
			glUniform4f(location, newValue.x, newValue.y, newValue.z, newValue.w);
		}
		else if constexpr (std::is_same_v<ValueType, glm::mat4x4>) {
			glUniformMatrix4fv(location, 1, false, &newValue[0][0]);
		}
	}
};