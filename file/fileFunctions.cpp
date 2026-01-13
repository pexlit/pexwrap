#include "fileFunctions.h"
#include <string>
#include <filesystem>
#include <fstream>
bool readStringFromFile(const std::filesystem::path &file_path, std::string &content)
{
	const std::ifstream input_stream(file_path, std::ios_base::binary);
	if (input_stream.fail())
	{
		return false;
	}

	std::stringstream buffer;
	buffer << input_stream.rdbuf();

	content = buffer.str();
	return true;
}
