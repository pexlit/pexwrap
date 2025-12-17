#pragma once
#include "safeInclude/includeGL.h"
#include <glm/vec4.hpp>
#include <type_traits>
#include <glm/mat4x4.hpp>
struct Uniform
{
	// location in the shader
	GLuint location;
	GLuint shaderProgramID;
	GLint size;
	std::string name;
	// float, vec3 or mat4, etc
	GLenum type;
	template <typename ValueType>
	void setValue(const ValueType &newValue)
	{
		if constexpr (std::is_same_v<ValueType, glm::vec4>)
		{
			glUniform4fv(location, 1, &newValue[0]);
		}
		else if constexpr (std::is_same_v<ValueType, glm::vec3>)
		{
			glUniform3fv(location, 1, &newValue[0]);
		}
		else if constexpr (std::is_same_v<ValueType, int>)
		{
			glUniform1i(location, newValue);
		}
		else if constexpr (std::is_same_v<ValueType, glm::mat4x4>)
		{
			glUniformMatrix4fv(location, 1, false, &newValue[0][0]);
		}
		else if constexpr (std::is_same_v<ValueType, float>)
		{
			glUniform1fv(location, 1, &newValue);
		}
		else
		{
			static_assert(false, "type not implemented");
		}
	}
};
