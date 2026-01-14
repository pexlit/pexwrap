#pragma once
#include <string>
#include <vector>

struct StringHierarchy {
	char charachter;
	size_t start;
	size_t end{};
	std::vector<StringHierarchy *> children{};
	StringHierarchy(char charachter, size_t start) : charachter(charachter), start(start) {}
	~StringHierarchy();
	StringHierarchy* cloneWithOffset(size_t offset);
};