
#include "PPU.hpp"
#include "LogUtil.hpp"

void PPU::drawLine() {
    // Read last drew line from LY register
    uint8_t line = memory->read8(LY, false);
    // Increment for new line and modulo 154 to reset if necessary
    line = (line+1)%154;
    // Write the current line back into LY
    memory->write8(LY, line);

    if (line > 144) {
        // We are in VBlank
        return;
    }

    // Go on drawing the tiles pixels for this line
    int rowInTilesMap = line/8; // Each tile is 8 px tall
    bool hasTile = false;
    uint16_t tileMapAddress = bgTilesMapAddress();
    for (int x = 0; x < 160; x++) {
        int colInTilesMap = x/8; // Each tile is 8 px wide
        uint16_t tileNumberAddress = tileMapAddress + (rowInTilesMap*32)+colInTilesMap;
        uint8_t tileNumber = memory->read8(tileNumberAddress, false);

        if (tileNumber != 0) {
            hasTile = true;
            uint16_t tileAddress = addressForTile(tileNumber);       // Retrieve tile

            int xInTile = x%8, yInTile = line%8;                     // Calculate positions in tile for current px
            uint16_t yOffsetInTile = yInTile*2;                      // Two bytes per row
            uint8_t xBitInTileBytes = (7-xInTile);                   // msb is first pixel, lsb is last pixel

            uint8_t lsb = memory->read8(tileAddress+yOffsetInTile, false);
            uint8_t msb = memory->read8(tileAddress+yOffsetInTile+1, false);

            uint8_t mask = 1 << xBitInTileBytes;
            uint8_t msbc = (msb & mask) >> (xBitInTileBytes-1);
            uint8_t lsbc = (lsb & mask) >> xBitInTileBytes;
            uint8_t color = msbc | lsbc;

            // if (color == 1) cout << "O";
            // else if (color == 2) cout << "x";
            // else if (color == 3) cout << "X";
            // else if (color == 0) cout << " ";
        }
    }

    // if (hasTile) cout << endl;
}

// Note: opposite of what the gameboy manual says
uint16_t PPU::bgTilesDataAddress() {
    uint8_t lcdc = memory->read8(LCDC, false);
    return !isBitSet(lcdc, LCDC_BG_AND_WIN_TILE_DATA_SELECT) ? 0x8000 : 0x8800;
}

// Note: opposite of what the gameboy manual says
uint16_t PPU::addressForTile(int8_t tileNumber) {
    uint8_t lcdc = memory->read8(LCDC, false);
    if (!isBitSet(lcdc, LCDC_BG_AND_WIN_TILE_DATA_SELECT)) {
        // Unsigned offset
        uint16_t uTileNumber = (uint16_t)tileNumber;
        uint16_t uTileOffset = uTileNumber * 16;
        return bgTilesDataAddress() + uTileOffset;
    } else {
        // Signed offset
        int16_t sTileNumber = tileNumber;
        int16_t sTileOffset = sTileNumber * 16;
        return bgTilesDataAddress() + sTileOffset;
    }
}

uint16_t PPU::bgTilesMapAddress() {
    uint8_t lcdc = memory->read8(LCDC, false);
    return isBitSet(lcdc, LCDC_BG_TILE_MAP_DISPLAY_SELECT) ? 0x9C00 : 0x9800;
}