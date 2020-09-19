#include "BlockManager.h"

void BlockManager::getTextureFromId(unsigned char id, glm::vec2 array[BLOCK_FACE_SIZE][TEX_COORD_SIZE]) {
    if (id == BlockManager::BLOCK_GRASS) {
        array[Top][TopLeft] = grassTopTexture[TopLeft];
        array[Top][TopRight] = grassTopTexture[TopRight];
        array[Top][BottomLeft] = grassTopTexture[BottomLeft];
        array[Top][BottomRight] = grassTopTexture[BottomRight];

        array[Bottom][TopLeft] = dirtTexture[TopLeft];
        array[Bottom][TopRight] = dirtTexture[TopRight];
        array[Bottom][BottomLeft] = dirtTexture[BottomLeft];
        array[Bottom][BottomRight] = dirtTexture[BottomRight];

        array[Left][TopLeft] = grassSideTexture[TopLeft];
        array[Left][TopRight] = grassSideTexture[TopRight];
        array[Left][BottomLeft] = grassSideTexture[BottomLeft];
        array[Left][BottomRight] = grassSideTexture[BottomRight];

        array[Right][TopLeft] = grassSideTexture[TopLeft];
        array[Right][TopRight] = grassSideTexture[TopRight];
        array[Right][BottomLeft] = grassSideTexture[BottomLeft];
        array[Right][BottomRight] = grassSideTexture[BottomRight];

        array[Front][TopLeft] = grassSideTexture[TopLeft];
        array[Front][TopRight] = grassSideTexture[TopRight];
        array[Front][BottomLeft] = grassSideTexture[BottomLeft];
        array[Front][BottomRight] = grassSideTexture[BottomRight];

        array[Back][TopLeft] = grassSideTexture[TopLeft];
        array[Back][TopRight] = grassSideTexture[TopRight];
        array[Back][BottomLeft] = grassSideTexture[BottomLeft];
        array[Back][BottomRight] = grassSideTexture[BottomRight];
    } else if (id == BlockManager::BLOCK_DIRT) {
        for (int i = 0; i < BLOCK_FACE_SIZE; i++) {
            array[i][TopLeft] = dirtTexture[TopLeft];
            array[i][TopRight] = dirtTexture[TopRight];
            array[i][BottomLeft] = dirtTexture[BottomLeft];
            array[i][BottomRight] = dirtTexture[BottomRight];
        }
    } else if (id == BlockManager::BLOCK_STONE) {
        for (int i = 0; i < BLOCK_FACE_SIZE; i++) {
            array[i][TopLeft] = stoneTexture[TopLeft];
            array[i][TopRight] = stoneTexture[TopRight];
            array[i][BottomLeft] = stoneTexture[BottomLeft];
            array[i][BottomRight] = stoneTexture[BottomRight];
        }
    } else if (id == BlockManager::BLOCK_WATER) {
        for (int i = 0; i < BLOCK_FACE_SIZE; i++) {
            array[i][TopLeft] = waterTexture[TopLeft];
            array[i][TopRight] = waterTexture[TopRight];
            array[i][BottomLeft] = waterTexture[BottomLeft];
            array[i][BottomRight] = waterTexture[BottomRight];
        }
    } else {
        for (int i = 0; i < BLOCK_FACE_SIZE; i++) {
            array[i][TopLeft] = unknownTexture[TopLeft];
            array[i][TopRight] = unknownTexture[TopRight];
            array[i][BottomLeft] = unknownTexture[BottomLeft];
            array[i][BottomRight] = unknownTexture[BottomRight];
        }
    }
}
