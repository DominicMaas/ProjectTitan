#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H  
#include "Logger.h"
#include "../Shader.h"
#include <map>

class TextRenderer {
private:
	Shader _textShader;
	FT_Library _font;
	FT_Face _face;
	unsigned int _vbo;
	unsigned int _vao;

	/// Holds all state information relevant to a character as loaded using FreeType
	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
		unsigned int Advance;    // Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> Characters;

public:
	TextRenderer(glm::mat4 proj);

	void renderText(std::string text, glm::vec2 position, float scale, glm::vec3 color);
};