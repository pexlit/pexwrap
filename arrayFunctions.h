#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <string>
template<typename T>
inline static bool contains(const std::vector<T>& array, const T& value) {
	return std::ranges::find(array.begin(), array.end(), value) != array.end();
}
template<typename T, typename FunctionType>
inline static bool contains_if(const std::vector<T>& array, FunctionType function) {
	return std::ranges::find_if(array.begin(), array.end(), function) != array.end();
}

//will return array.size if the element is not present.
template<typename T>
inline static size_t indexOf(const std::vector<T>& array, const T& value) {
	return std::distance(array.begin(), std::ranges::find(array.begin(), array.end(), value));
}

//will return array.size if the element is not present.
template<typename T, typename FunctionType>
inline static size_t indexOf_if(const std::vector<T>& array, FunctionType function) {
	return std::distance(array.begin(), std::ranges::find_if(array.begin(), array.end(), function));
}

//swap indexes <=> values
template<typename outT, typename T, size_t N>
constexpr static std::array<outT, N> transposeArray(const std::array<T, N>& in) {
	std::array<outT, N> transposed{};
	for (size_t i = 0; i != in.size(); i++) {
		transposed[(size_t)in[i]] = (outT)i;
	}
	return transposed;
}

template<size_t N>
constexpr static void copyToArray(const std::string& s, std::array<char, N>& array) {
	size_t sizeToCopy = glm::min(s.size(), array.size() - 1);
	std::copy(s.begin(), s.begin() + sizeToCopy, array.data());
	
	array[sizeToCopy] = 0;
}