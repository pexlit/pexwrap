#include "shader.h"
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "fileFunctions.h"


Shader::Shader(const std::filesystem::path& path)
{
	type = (path.extension() == ".frag") ? ShaderType::Fragment : ShaderType::Vertex;

	ID = glCreateShader((GLuint)type);
	const std::string shaderCode = read_string_from_file(path);
	const char* temporaryPointer = shaderCode.c_str();
	glShaderSource(ID, 1, &temporaryPointer, NULL);
	glCompileShader(ID);

	int  compileSuccess;
	char infoLog[512];
	glGetShaderiv(ID, GL_COMPILE_STATUS, &compileSuccess);
	success = compileSuccess;
	if (!success)
	{
		glGetShaderInfoLog(ID, 512, NULL, infoLog);
		std::cout << "Error while compiling shader" + path.string() + "\n" << infoLog << std::endl;
	}
}
