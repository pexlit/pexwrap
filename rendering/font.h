#pragma once
#include "safeInclude/includeGL.h"
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/vec2.hpp>
#include <map>
#include "shaderProgram.h"
#include <string>
#include <glm/vec3.hpp>

constexpr int fontPixelSize = 48;
bool initializeFont();

void RenderText(ShaderProgram& shader, std::string text, glm::vec2 pos00, float scale, glm::vec3 color);

float measureStringSize(const std::string& text);

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};