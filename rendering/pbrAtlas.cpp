#ifdef PEXLIT_GL
#include "pbrAtlas.h"
#include "texture.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include <cmath>
#include <iostream>
#include <vector>

static PBRAtlas globalAtlas;

namespace
{
std::string findPbrTextureFile(const std::filesystem::path &materialDir, const char *suffix)
{
	if (!std::filesystem::exists(materialDir))
		return {};

	for (const auto &entry : std::filesystem::directory_iterator(materialDir)) {
		const std::string filename = entry.path().filename().string();
		if (filename.find(suffix) != std::string::npos)
			return entry.path().string();
	}

	return {};
}
}

PBRAtlas &getPBRAtlas() { return globalAtlas; }

PBRAtlas::~PBRAtlas() {
	// Resources should be cleaned up via cleanup() before OpenGL context destruction
	// If not, we can't safely call glDeleteTextures here
}

void PBRAtlas::cleanup() {
	if (materialArray) {
		glDeleteTextures(1, &materialArray);
		materialArray = 0;
	}
	if (normalArray) {
		glDeleteTextures(1, &normalArray);
		normalArray = 0;
	}

	for (auto &[name, previewTexture] : previewTextures) {
		(void)name;
		delete previewTexture;
	}
	previewTextures.clear();
}

void PBRAtlas::init(const std::filesystem::path &sourceDir, const std::filesystem::path &cacheDir) {
	static const std::vector<std::pair<std::string, std::string>> defaultMaterials = {
		{"grass", "grass"}, {"soil", "ground"}, {"gravel", "gravel"}, {"snow", "snow"}
	};
	init(sourceDir, cacheDir, defaultMaterials);
}

void PBRAtlas::init(
	const std::filesystem::path &sourceDir,
	const std::filesystem::path &cacheDir,
	const std::vector<std::pair<std::string, std::string>> &materials)
{
	configureMaterials(materials);

	// Try to load from cache first
	if (loadFromCache(cacheDir, materials)) {
		loadPreviewTextures(sourceDir, materials);
		std::cout << "Loaded PBR atlas from cache\n";
		return;
	}

	// Otherwise pack from source
	std::cout << "Packing PBR textures...\n";
	packTextures(sourceDir, cacheDir, materials);
	loadPreviewTextures(sourceDir, materials);
}

const std::vector<std::string> &PBRAtlas::getMaterialNames() const
{
	return materialNames;
}

Texture *PBRAtlas::getMaterialPreviewTexture(const std::string &name) const
{
	auto it = previewTextures.find(name);
	if (it != previewTextures.end())
		return it->second;
	return nullptr;
}

void PBRAtlas::bind(int materialUnit, int normalUnit) const {
	glActiveTexture(GL_TEXTURE0 + materialUnit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, materialArray);
	glActiveTexture(GL_TEXTURE0 + normalUnit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, normalArray);
}

void PBRAtlas::configureMaterials(const std::vector<std::pair<std::string, std::string>> &materials)
{
	materialNames.clear();
	materialNames.reserve(materials.size());

	for (int materialIndex = 0; materialIndex < static_cast<int>(materials.size()); ++materialIndex) {
		const std::string &name = materials[static_cast<std::size_t>(materialIndex)].first;
		materialNames.push_back(name);
	}

	materialCount = static_cast<int>(materials.size());
}

void PBRAtlas::loadPreviewTextures(
	const std::filesystem::path &sourceDir,
	const std::vector<std::pair<std::string, std::string>> &materials)
{
	for (auto &[name, previewTexture] : previewTextures) {
		(void)name;
		delete previewTexture;
	}
	previewTextures.clear();

	for (const auto &[name, folder] : materials) {
		const std::string colorFile = findPbrTextureFile(sourceDir / folder, "_Color.png");
		if (colorFile.empty())
			continue;

		previewTextures[name] = new Texture(colorFile);
	}
}

bool PBRAtlas::loadFromCache(
	const std::filesystem::path &cacheDir,
	const std::vector<std::pair<std::string, std::string>> &materials)
{
	std::filesystem::path materialPath = cacheDir / "material_atlas.png";
	std::filesystem::path normalPath = cacheDir / "normal_atlas.png";

	if (!std::filesystem::exists(materialPath) || !std::filesystem::exists(normalPath))
		return false;

	// Load material atlas (RGBA, stacked vertically: 3 materials * 1024 = 3072 height)
	int w, h, channels;
	stbi_set_flip_vertically_on_load(false);
	unsigned char *materialData = stbi_load(materialPath.string().c_str(), &w, &h, &channels, 4);
	if (!materialData || w != atlasSize) {
		if (materialData)
			stbi_image_free(materialData);
		return false;
	}

	materialCount = h / atlasSize;
	if (materialCount != static_cast<int>(materials.size())) {
		stbi_image_free(materialData);
		return false;
	}

	// Load normal atlas
	unsigned char *normalData = stbi_load(normalPath.string().c_str(), &w, &h, &channels, 3);
	if (!normalData || w != atlasSize || h != materialCount * atlasSize) {
		stbi_image_free(materialData);
		if (normalData)
			stbi_image_free(normalData);
		return false;
	}

	// Create array textures
	glGenTextures(1, &materialArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, materialArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, atlasSize, atlasSize, materialCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	for (int i = 0; i < materialCount; ++i) {
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, atlasSize, atlasSize, 1, GL_RGBA, GL_UNSIGNED_BYTE,
			materialData + i * atlasSize * atlasSize * 4
		);
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &normalArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, normalArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, atlasSize, atlasSize, materialCount, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	for (int i = 0; i < materialCount; ++i) {
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, atlasSize, atlasSize, 1, GL_RGB, GL_UNSIGNED_BYTE,
			normalData + i * atlasSize * atlasSize * 3
		);
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(materialData);
	stbi_image_free(normalData);

	return true;
}

void PBRAtlas::packTextures(
	const std::filesystem::path &sourceDir,
	const std::filesystem::path &cacheDir,
	const std::vector<std::pair<std::string, std::string>> &materials)
{
	materialCount = (int)materials.size();

	// Allocate combined data for cache saving
	std::vector<unsigned char> materialAtlasData(atlasSize * atlasSize * 4 * materialCount);
	std::vector<unsigned char> normalAtlasData(atlasSize * atlasSize * 3 * materialCount);

	for (int matIdx = 0; matIdx < materialCount; ++matIdx) {
		const auto &[name, folder] = materials[matIdx];

		std::filesystem::path matDir = sourceDir / folder;

		// Find texture files (they have prefixes like Grass005_1K-PNG_)
		const std::string colorFile = findPbrTextureFile(matDir, "_Color.png");
		const std::string roughnessFile = findPbrTextureFile(matDir, "_Roughness.png");
		const std::string aoFile = findPbrTextureFile(matDir, "_AmbientOcclusion.png");
		const std::string displacementFile = findPbrTextureFile(matDir, "_Displacement.png");
		const std::string normalFile = findPbrTextureFile(matDir, "_NormalGL.png");

		// Load textures
		int w, h, c;
		stbi_set_flip_vertically_on_load(false);

		unsigned char *colorData = stbi_load(colorFile.c_str(), &w, &h, &c, 3);
		unsigned char *roughnessData = stbi_load(roughnessFile.c_str(), &w, &h, &c, 1);
		unsigned char *aoData = stbi_load(aoFile.c_str(), &w, &h, &c, 1);
		unsigned char *displacementData = stbi_load(displacementFile.c_str(), &w, &h, &c, 1);
		unsigned char *normalData = stbi_load(normalFile.c_str(), &w, &h, &c, 3);

		if (!colorData || !roughnessData || !aoData || !displacementData || !normalData) {
			std::cerr << "Failed to load textures for material: " << name << "\n";
			if (colorData)
				stbi_image_free(colorData);
			if (roughnessData)
				stbi_image_free(roughnessData);
			if (aoData)
				stbi_image_free(aoData);
			if (displacementData)
				stbi_image_free(displacementData);
			if (normalData)
				stbi_image_free(normalData);
			continue;
		}

		// Pack into atlas: Grayscale (R) + Roughness (G) + AO (B) + Height (A)
		unsigned char *matDest = materialAtlasData.data() + matIdx * atlasSize * atlasSize * 4;
		unsigned char *normDest = normalAtlasData.data() + matIdx * atlasSize * atlasSize * 3;

		for (int i = 0; i < atlasSize * atlasSize; ++i) {
			// Convert color to grayscale (luminance)
			float r = colorData[i * 3 + 0] / 255.0f;
			float g = colorData[i * 3 + 1] / 255.0f;
			float b = colorData[i * 3 + 2] / 255.0f;
			float grayscale = 0.2126f * r + 0.7152f * g + 0.0722f * b;

			matDest[i * 4 + 0] = (unsigned char)(grayscale * 255.0f);
			matDest[i * 4 + 1] = roughnessData[i];
			matDest[i * 4 + 2] = aoData[i];
			matDest[i * 4 + 3] = displacementData[i];

			normDest[i * 3 + 0] = normalData[i * 3 + 0];
			normDest[i * 3 + 1] = normalData[i * 3 + 1];
			normDest[i * 3 + 2] = normalData[i * 3 + 2];
		}

		stbi_image_free(colorData);
		stbi_image_free(roughnessData);
		stbi_image_free(aoData);
		stbi_image_free(displacementData);
		stbi_image_free(normalData);

		std::cout << "Packed material: " << name << "\n";
	}

	// Create GL textures
	glGenTextures(1, &materialArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, materialArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, atlasSize, atlasSize, materialCount, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	for (int i = 0; i < materialCount; ++i) {
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, atlasSize, atlasSize, 1, GL_RGBA, GL_UNSIGNED_BYTE,
			materialAtlasData.data() + i * atlasSize * atlasSize * 4
		);
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenTextures(1, &normalArray);
	glBindTexture(GL_TEXTURE_2D_ARRAY, normalArray);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB8, atlasSize, atlasSize, materialCount, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

	for (int i = 0; i < materialCount; ++i) {
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, atlasSize, atlasSize, 1, GL_RGB, GL_UNSIGNED_BYTE,
			normalAtlasData.data() + i * atlasSize * atlasSize * 3
		);
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Save to cache
	std::filesystem::create_directories(cacheDir);

	// Save as vertically stacked image (each layer below the previous)
	stbi_write_png(
		(cacheDir / "material_atlas.png").string().c_str(), atlasSize, atlasSize * materialCount, 4, materialAtlasData.data(),
		atlasSize * 4
	);
	stbi_write_png(
		(cacheDir / "normal_atlas.png").string().c_str(), atlasSize, atlasSize * materialCount, 3, normalAtlasData.data(),
		atlasSize * 3
	);

	std::cout << "Saved PBR atlas to cache\n";
}
#endif
