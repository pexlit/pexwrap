#pragma once
#ifdef PEXLIT_GLM
#include "debugViewMode.h"
#include "debugWireframeMode.h"
#include "frustum.h"
#include "transparentSphereRenderData.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <stdexcept>
#include <vector>

namespace renderContextDetail
{
	template<class T>
	const void* renderServiceKey()
	{
		static const char key{};
		return &key;
	}
}

struct RenderServiceEntry
{
	const void* key{};
	void* service{};
};

struct RenderServiceRegistry
{
	template<class T>
	void set(T& service)
	{
		const void* key = renderContextDetail::renderServiceKey<T>();
		for (RenderServiceEntry& entry : entries)
		{
			if (entry.key == key)
			{
				entry.service = &service;
				return;
			}
		}
		entries.push_back(RenderServiceEntry{.key = key, .service = &service});
	}

	template<class T>
	T* get() const
	{
		const void* key = renderContextDetail::renderServiceKey<T>();
		for (const RenderServiceEntry& entry : entries)
		{
			if (entry.key == key)
				return static_cast<T*>(entry.service);
		}
		return nullptr;
	}

	template<class T>
	T& require() const
	{
		if (T* service = get<T>())
			return *service;
		throw std::runtime_error("missing required render service");
	}

private:
	std::vector<RenderServiceEntry> entries;
};

// Maximum render distance before scaling kicks in (for depth buffer precision)
constexpr double maxRenderDistance = 0b10000000000000000;

// Compute scale factor to keep objects within depth buffer precision range.
// Returns scale factor (1.0 if close enough, < 1.0 if far away).
inline double computeDistanceScale(double distance) {
	return (distance > maxRenderDistance) ? maxRenderDistance / distance : 1.0;
}

// Compute scaled position and scale factor for rendering distant objects.
// Both the position and any sizes should be multiplied by the returned scale.
inline double computeScaledPosition(const glm::dvec3 &relativePos, glm::vec3 &outScaledPos) {
	double distance = glm::length(relativePos);
	double scale = computeDistanceScale(distance);
	outScaledPos = glm::vec3(relativePos * scale);
	return scale;
}

// Shared data for rendering from the camera's point of view.
struct RenderContext {
	glm::mat4 worldToScreen;
	glm::mat4 screenRayToWorld{1.0f}; // Camera local ray basis without translation, for screen-space ray reconstruction
	glm::dvec3 cameraPosition;
	Frustum frustum;
	float time{};
	float screenHeight{};
	float fovY{};
	float nearPlane{0.1f}; // Near plane distance for depth linearization
	float ambientLight{};  // Optional ambient floor for debugger-style preview lighting
	bool lightingEnabled{true};
	DebugViewMode debugViewMode{};
	bool wireframePass{};
	bool backfaceCullingEnabled{true};
	bool showStarChunks{};
	bool showTerrainChunks{};
	RenderServiceRegistry* services{};
	unsigned int sceneDepthTexture{}; // Scene depth texture for water/atmosphere shaders
	unsigned int sceneColorTexture{}; // Copied scene color for shader-owned transparent compositing
	glm::ivec2 screenSize{};		  // Screen dimensions for depth texture sampling

	// Deferred transparent sphere rendering - collected during opaque pass, rendered later
	mutable DeferredTransparentSpheres deferredTransparentSpheres;

	glm::mat4 getBodyTransform(const glm::dvec3 &bodyPosition) const {
		glm::vec3 localPos = glm::vec3(bodyPosition - cameraPosition);
		glm::mat4 translateMat = glm::translate(glm::mat4(1.0f), localPos);
		return worldToScreen * translateMat;
	}

	template<class T>
	T* service() const
	{
		return services ? services->get<T>() : nullptr;
	}

	template<class T>
	T& requireService() const
	{
		if (!services)
			throw std::runtime_error("missing render service registry");
		return services->require<T>();
	}
};
#endif
