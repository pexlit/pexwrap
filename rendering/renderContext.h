#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Shared data for rendering from the camera's point of view.
struct RenderContext
{
	glm::mat4 worldToScreen;
	glm::dvec3 cameraPosition;
	float time{};
	float screenHeight{};
	float fovY{};
	int debugViewMode{};
	int wireframePass{};

	glm::mat4 getBodyTransform(const glm::dvec3 &bodyPosition) const
	{
		glm::vec3 localPos = glm::vec3(bodyPosition - cameraPosition);
		glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), localPos);
		return worldToScreen * translateMat;
	}
};
