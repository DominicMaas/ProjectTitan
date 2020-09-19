#pragma once

#include <pch.h>

class BlockManager {
public:
    static const int TEX_COORD_SIZE = 4;
    static const int BLOCK_FACE_SIZE = 6;

    constexpr static const float TEX_X_STEP = 0.0625;

    static void getTextureFromId(unsigned char id, glm::vec2 array[BLOCK_FACE_SIZE][TEX_COORD_SIZE]);

    static const unsigned char BLOCK_AIR = 0;
    static const unsigned char BLOCK_GRASS = 1;
    static const unsigned char BLOCK_DIRT = 2;
    static const unsigned char BLOCK_STONE = 3;
    static const unsigned char BLOCK_WATER = 4;

    enum TexCoord {
        TopRight = 0,
        BottomRight,
        BottomLeft,
        TopLeft
    };

    enum BlockFace {
        Top = 0,
        Bottom,
        Left,
        Right,
        Front,
        Back
    };

private:


    constexpr static const glm::vec2 grassSideTexture[4] = {
            glm::vec2(TEX_X_STEP * 2, 1.0f), // TopRight
            glm::vec2(TEX_X_STEP * 2, 0.0f), // BottomRight
            glm::vec2(TEX_X_STEP * 1, 0.0f), // BottomLeft
            glm::vec2(TEX_X_STEP * 1, 1.0f), // TopLeft
    };

    constexpr static const glm::vec2 grassTopTexture[4] = {
            glm::vec2(TEX_X_STEP * 3, 1.0f), // TopRight
            glm::vec2(TEX_X_STEP * 3, 0.0f), // BottomRight
            glm::vec2(TEX_X_STEP * 2, 0.0f), // BottomLeft
            glm::vec2(TEX_X_STEP * 2, 1.0f), // TopLeft
    };

    constexpr static const glm::vec2 dirtTexture[4] = {
            glm::vec2(TEX_X_STEP * 1, 1.0f), // TopRight
            glm::vec2(TEX_X_STEP * 1, 0.0f), // BottomRight
            glm::vec2(TEX_X_STEP * 0, 0.0f), // BottomLeft
            glm::vec2(TEX_X_STEP * 0, 1.0f), // TopLeft
    };

    constexpr static const glm::vec2 stoneTexture[4] = {
            glm::vec2(TEX_X_STEP * 6, 1.0f), // TopRight
            glm::vec2(TEX_X_STEP * 6, 0.0f), // BottomRight
            glm::vec2(TEX_X_STEP * 5, 0.0f), // BottomLeft
            glm::vec2(TEX_X_STEP * 5, 1.0f), // TopLeft
    };

    constexpr static const glm::vec2 waterTexture[4] = {
            glm::vec2(TEX_X_STEP * 8, 1.0f), // TopRight
            glm::vec2(TEX_X_STEP * 8, 0.0f), // BottomRight
            glm::vec2(TEX_X_STEP * 7, 0.0f), // BottomLeft
            glm::vec2(TEX_X_STEP * 7, 1.0f), // TopLeft
    };

    constexpr static const glm::vec2 unknownTexture[4] = {
            glm::vec2(TEX_X_STEP * 14, 1.0f), // TopRight
            glm::vec2(TEX_X_STEP * 14, 0.0f), // BottomRight
            glm::vec2(TEX_X_STEP * 12, 0.0f), // BottomLeft
            glm::vec2(TEX_X_STEP * 12, 1.0f), // TopLeft
    };

    /*
     *  topRight += glm::vec2(-0.875f, 1.0f);
                bottomRight += glm::vec2(-0.875f, -1.0f);
                bottomLeft += glm::vec2(0.1875f, -1.0f);
                topLeft += glm::vec2(0.1875f, 1.0f);
     *
     *
     */
};
