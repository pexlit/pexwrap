#ifdef PEXLIT_GL
#include "model/ddsTexture.h"
#include "texture.h"
#include "stb_image.h"

namespace
{
DdsTextureUploadObserver ddsTextureUploadObserver;
}

void setDdsTextureUploadObserverForTests(DdsTextureUploadObserver observer)
{
	ddsTextureUploadObserver = std::move(observer);
}

void resetDdsTextureUploadObserverForTests()
{
	ddsTextureUploadObserver = {};
}

Texture::Texture(GLuint width, GLuint height, uint8_t *data)
	: data(data ? data : new uint8_t[3 * width * height]), width(width), height(height) {
	std::fill(this->data, this->data + 3 * width * height, 0xff);
	toGL();
}

Texture::Texture(
	GLuint width, GLuint height, const uint8_t *srcData, GLenum internalFormat, GLenum format, GLenum type, bool generateMipmaps
)
	: data(nullptr), width(width), height(height), ID(0) {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, srcData);
	if (generateMipmaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Texture::Texture(const DdsTextureData &ddsTexture)
	: data(nullptr), width(ddsTexture.width), height(ddsTexture.height), ID(0) {
	if (ddsTextureUploadObserver) {
		ddsTextureUploadObserver();
	}
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	const GLenum internalFormat = glInternalFormatForDdsCompressionFormat(ddsTexture.format);
	for (size_t mipIndex = 0; mipIndex < ddsTexture.mipLevels.size(); ++mipIndex) {
		const DdsMipLevel &mipLevel = ddsTexture.mipLevels[mipIndex];
		glCompressedTexImage2D(
			GL_TEXTURE_2D,
			(GLint)mipIndex,
			internalFormat,
			(GLsizei)mipLevel.width,
			(GLsizei)mipLevel.height,
			0,
			(GLsizei)mipLevel.bytes.size(),
			mipLevel.bytes.data()
		);
	}
	glTexParameteri(
		GL_TEXTURE_2D,
		GL_TEXTURE_MIN_FILTER,
		ddsTexture.mipLevels.size() > 1 ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

Texture::Texture(const std::filesystem::path &path) {
	int channelCount, desiredChannelCount = 3;
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path.string().c_str(), (int *)&width, (int *)&height, &channelCount, desiredChannelCount);
	toGL();
}

Texture::~Texture() { glDeleteTextures(1, &ID); }

void Texture::setFiltering(GLenum minFilter, GLenum magFilter) {
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::toGL() {
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
#endif
