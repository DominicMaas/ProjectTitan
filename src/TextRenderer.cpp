#include <constants.h>
#include "TextRenderer.h"
#include "spdlog/spdlog.h"

TextRenderer::TextRenderer(glm::mat4 proj) : _textShader("shaders/text_shader.vert", "shaders/text_shader.frag") {
    _textShader.use();
    _textShader.setMat4("projection", proj);

    if (FT_Init_FreeType(&_font)) {
        spdlog::warn("[Text Renderer] Warning! Could not init FreeType Library");
        return;
    }

    // Load font as face
    if (FT_New_Face(_font, "fonts/arial.ttf", 0, &_face)) {
        spdlog::warn("[Text Renderer] Warning! Failed to load font");
        return;
    }

    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(_face, 0, 48);

    // Disable byte-alignment restriction
    GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++) {
        // Load character glyph
        if (FT_Load_Char(_face, c, FT_LOAD_RENDER)) {
            spdlog::error("[Text Renderer] Failed to load Glyph");
            continue;
        }

        // Generate texture
        GLuint texture;
        GLCall(glGenTextures(1, &texture));
        GLCall(glBindTexture(GL_TEXTURE_2D, texture));
        GLCall(glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                _face->glyph->bitmap.width,
                _face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                _face->glyph->bitmap.buffer
        ));

        // Set texture options
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

        // Now store character for later use
        Character character = {
                texture,
                glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
                glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
                static_cast<unsigned int>(_face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    // Destroy FreeType once we're finished
    FT_Done_Face(_face);
    FT_Done_FreeType(_font);

    // Configure VAO/VBO for texture quads
    GLCall(glGenVertexArrays(1, &_vao));
    GLCall(glGenBuffers(1, &_vbo));
    GLCall(glBindVertexArray(_vao));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));
}

void TextRenderer::renderText(std::string text, glm::vec2 position, float scale, glm::vec3 color) {
    GLCall(glDisable(GL_DEPTH_TEST));

    // Activate corresponding render state
    _textShader.use();
    _textShader.setColor("textColor", color);
    GLCall(glActiveTexture(GL_TEXTURE0));
    GLCall(glBindVertexArray(_vao));

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++) {
        Character ch = Characters[*c];

        GLfloat xpos = position.x + ch.Bearing.x * scale;
        GLfloat ypos = position.y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
                {xpos,     ypos + h, 0.0, 0.0},
                {xpos,     ypos,     0.0, 1.0},
                {xpos + w, ypos,     1.0, 1.0},

                {xpos,     ypos + h, 0.0, 0.0},
                {xpos + w, ypos,     1.0, 1.0},
                {xpos + w, ypos + h, 1.0, 0.0}
        };
        // Render glyph texture over quad
        GLCall(glBindTexture(GL_TEXTURE_2D, ch.TextureID));
        // Update content of VBO memory
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, _vbo));
        GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices),
                        vertices)); // Be sure to use glBufferSubData and not glBufferData

        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        // Render quad
        GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        position.x += (ch.Advance >> 6) *
                      scale; // Bit-shift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    GLCall(glBindVertexArray(0));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));

    GLCall(glEnable(GL_DEPTH_TEST));
}
