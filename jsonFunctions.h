#pragma once
#include <glm/vec2.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
namespace glm {
	inline void to_json(json& j, const glm::vec2& vec) {
		j = json{ {"x", vec.x}, {"y", vec.y} };
	}

	inline void from_json(const json& j, glm::vec2& vec) {
		j["x"].get_to(vec.x);
		j["y"].get_to(vec.y);
	}
}