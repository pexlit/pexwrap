#include "textureUpload.h"

#include <cstddef>
#include <stb_image.h>

namespace
{
GLuint uploadFile(
    const std::filesystem::path& path,
    int components,
    GLint internalFormat,
    GLenum format,
    GLenum wrap)
{
    int width = 0;
    int height = 0;
    int sourceComponents = 0;
    unsigned char* pixels = stbi_load(
        path.string().c_str(),
        &width,
        &height,
        &sourceComponents,
        components);
    if (pixels == nullptr || width <= 0 || height <= 0)
    {
        stbi_image_free(pixels);
        return 0;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
        format, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);
    return texture;
}
}

GLuint uploadSrgbTexture(const std::filesystem::path& path)
{
    return uploadFile(path, 3, GL_SRGB8, GL_RGB, GL_REPEAT);
}

GLuint uploadSrgbAlphaTexture(const std::filesystem::path& path, TextureWrap wrap)
{
	return uploadRgbaTexture(
		path,
		TextureFileColorSpace::Srgb,
		wrap);
}

GLuint uploadRgbaTexture(
	const std::filesystem::path& path,
	TextureFileColorSpace colorSpace,
	TextureWrap wrap)
{
	return uploadFile(
		path,
		4,
		colorSpace == TextureFileColorSpace::Srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8,
		GL_RGBA,
		wrap == TextureWrap::Repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
}

GLuint uploadLinearTexture(const std::filesystem::path& path, TextureWrap wrap)
{
	return uploadRgbaTexture(path, TextureFileColorSpace::Linear, wrap);
}

void destroyTexture(GLuint& texture)
{
	if (texture != 0)
		glDeleteTextures(1, &texture);
	texture = 0;
}

GLuint uploadStaticModelTexture(
    const StaticModelGeometry& geometry,
    int textureIndex,
    bool srgb)
{
    if (textureIndex < 0 || textureIndex >= static_cast<int>(geometry.textures.size()))
        return 0;
    const int imageIndex = geometry.textures[static_cast<std::size_t>(textureIndex)].imageIndex;
    if (imageIndex < 0 || imageIndex >= static_cast<int>(geometry.images.size()))
        return 0;
    const StaticModelImage& image = geometry.images[static_cast<std::size_t>(imageIndex)];
    if (image.width <= 0 || image.height <= 0 || image.pixels.empty())
        return 0;

    GLenum format = GL_RGBA;
    GLenum internalFormat = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
    if (image.component == 1)
    {
        format = GL_RED;
        internalFormat = GL_R8;
    }
    else if (image.component == 2)
    {
        format = GL_RG;
        internalFormat = GL_RG8;
    }
    else if (image.component == 3)
    {
        format = GL_RGB;
        internalFormat = srgb ? GL_SRGB8 : GL_RGB8;
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat),
        image.width, image.height, 0, format, GL_UNSIGNED_BYTE, image.pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}
