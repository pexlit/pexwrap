#include "texture.h"
#include "stb_image.h"

Texture::Texture(GLuint width, GLuint height, uint8_t* data) : data(data ? data : new uint8_t[3 * width * height]), width(width), height(height)
{
	std::fill(this->data, this->data + 3 * width * height, 0xff);
	toGL();
}

Texture::Texture(const std::filesystem::path& path)
{
	int channelCount, desiredChannelCount = 3;
	stbi_set_flip_vertically_on_load(true);
	data = stbi_load(path.string().c_str(), (int*)&width, (int*)&height, &channelCount, desiredChannelCount);
	toGL();
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::setPixel(GLuint x, GLuint y, uint32_t color)
{
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &color);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::toGL()
{
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, this->data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
