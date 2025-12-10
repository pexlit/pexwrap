#include "fileFunctions.h"
#include <string>
#include <filesystem>
#include <fstream>
std::string read_string_from_file(const std::filesystem::path& file_path) {
	const std::ifstream input_stream(file_path, std::ios_base::binary);
	if (input_stream.fail()) {
		throw std::runtime_error("Failed to open file");
	}

	std::stringstream buffer;
	buffer << input_stream.rdbuf();

	return buffer.str();
}