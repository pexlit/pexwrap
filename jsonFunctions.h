#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
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

	inline void to_json(json& j, const glm::vec3& vec) {
		j = json{ {"x", vec.x}, {"y", vec.y}, {"z", vec.z} };
	}

	inline void from_json(const json& j, glm::vec3& vec) {
		j["x"].get_to(vec.x);
		j["y"].get_to(vec.y);
		j["z"].get_to(vec.z);
	}

	inline void to_json(json& j, const glm::vec4& vec) {
		j = json{ {"x", vec.x}, {"y", vec.y}, {"z", vec.z}, {"w", vec.w} };
	}

	inline void from_json(const json& j, glm::vec4& vec) {
		j["x"].get_to(vec.x);
		j["y"].get_to(vec.y);
		j["z"].get_to(vec.z);
		j["w"].get_to(vec.w);
	}
}
