#pragma once
#ifdef PEXLIT_GL
#include "model/ddsTexture.h"
#include "safeInclude/includeGL.h"
#include <filesystem>
#include <functional>
#include <vector>

using DdsTextureUploadObserver = std::function<void()>;

void setDdsTextureUploadObserverForTests(DdsTextureUploadObserver observer);
void resetDdsTextureUploadObserverForTests();

struct Texture {
	uint8_t *data{};
	GLuint width{}, height{}, ID{};

	Texture(GLuint width, GLuint height, uint8_t *data = nullptr);
	Texture(const DdsTextureData &ddsTexture);
	Texture(
		GLuint width, GLuint height, const uint8_t *data, GLenum internalFormat, GLenum format, GLenum type,
		bool generateMipmaps
	);
	Texture(const std::filesystem::path &path);
	~Texture();
	void setFiltering(GLenum minFilter, GLenum magFilter);

  private:
	void toGL();
};
#endif
