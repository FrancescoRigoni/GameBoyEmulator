
#include "Devices/LCDRegs.hpp"
#include "Util/ByteUtil.hpp"

#define BACKGROUND_TILE_SIZE_BYTES 16
#define WINDOW_TILE_SIZE_BYTES 16

void LCDRegs::stateOAMSearch() {
    stat &= 0b11111100;
    stat |= 0b00000010;
}

void LCDRegs::stateDrawLine() {
    stat &= 0b11111100;
    stat |= 0b00000011;
}

void LCDRegs::stateHBlank() {
    stat &= 0b11111100;
}

void LCDRegs::stateVBlank() {
    stat &= 0b11111100;
    stat |= 0b00000001;
}

bool LCDRegs::inOAMSearch() {
    return (stat & 0b00000011) == 0b00000010;
}

bool LCDRegs::inDrawLine() {
    return (stat & 0b00000011) == 0b00000011;
}

bool LCDRegs::inHBlank() {
    return (stat & 0b00000011) == 0b00000000;
}

bool LCDRegs::inVBlank()  {
    return (stat & 0b00000011) == 0b00000001;
}

void LCDRegs::setCurrentLine(uint8_t line) {
    this->ly = line;
}

void LCDRegs::write8(uint16_t address, uint8_t value) {
    if (address == STAT) this->stat = value;
    else if (address == LCDC) this->lcdc = value;
    else if (address == SCY) this->scy = value;
    else if (address == SCX) this->scx = value;
    else if (address == LY) this->ly = 0;
    else if (address == LYC) this->lyc = value;
    else if (address == WIN_X) this->windowX = value;
    else if (address == WIN_Y) this->windowY = value;
    else {
        TRACE_IO(endl << "LCDRegs : Access to unhandled address " << cout16Hex(address) << endl);
    }
}

uint8_t LCDRegs::read8(uint16_t address) {
    if (address == STAT) return stat;
    else if (address == LCDC) return lcdc;
    else if (address == SCY) return scy;
    else if (address == SCX) return scx;
    else if (address == LY) return ly;
    else if (address == LYC) return lyc;
    else if (address == WIN_X) return windowX;
    else if (address == WIN_Y) return windowY;
    else {
        TRACE_IO(endl << "LCDRegs : Access to unhandled address " << cout16Hex(address) << endl);
        return 0;
    }
}

uint16_t LCDRegs::addressForBackgroundTile(uint8_t tileNumber) {
    bool dataSelect = isBitSet(lcdc, LCDC_BG_AND_WIN_TILE_DATA_SELECT_BIT);
    uint16_t baseAddress = dataSelect ? 0x8000 : 0x9000;

    if (dataSelect) {
        // Unsigned offset
        uint16_t uTileOffset = tileNumber * BACKGROUND_TILE_SIZE_BYTES;
        return baseAddress + uTileOffset;
    } else {
        // Signed offset
        int8_t sTileNumber = tileNumber;
        int16_t sTileOffset = (int16_t)sTileNumber * BACKGROUND_TILE_SIZE_BYTES;
        return baseAddress + sTileOffset;
    }
}

uint16_t LCDRegs::addressForBackgroundTilesMap() {
    return isBitSet(lcdc, LCDC_BG_TILE_MAP_DISPLAY_SELECT_BIT) ? 0x9C00 : 0x9800;
}

uint16_t LCDRegs::addressForWindowTile(uint8_t tileNumber) {
    return addressForBackgroundTile(tileNumber);
}

uint16_t LCDRegs::addressForWindowTilesMap() {
    return isBitSet(lcdc, LCDC_WIN_TILE_MAP_DISPLAY_SELECT_BIT) ? 0x9C00 : 0x9800;
}

uint16_t LCDRegs::addressForSprite(uint16_t spriteNumber) {
    // Number of the sprite * (8 rows) * (2 bytes per row (2bpp))
    uint16_t spriteOffset = spriteNumber*(8*2);
    return SPRITE_PATTERN_TABLE_START + spriteOffset;
}

uint16_t LCDRegs::addressForSpriteAttributeTable() {
    return SPRITE_ATTRIBUTE_TABLE_START;
}

bool LCDRegs::isScreenOn() {
    return isBitSet(lcdc, LCDC_CONTROL_OP_BIT);
}

bool LCDRegs::isBackgroundOn() {
    return isBitSet(lcdc, LCDC_BG_AND_WIN_DISPLAY_BIT);
}

bool LCDRegs::isWindowOn() {
    return isBitSet(lcdc, LCDC_BG_AND_WIN_DISPLAY_BIT) && isBitSet(lcdc, LCDC_WIN_DISPLAY_BIT);
}

bool LCDRegs::isSpritesOn() {
    return isBitSet(lcdc, LCDC_SPRITE_DISPLAY_BIT);
}

int LCDRegs::spriteHeightPx() {
    return isBitSet(lcdc, LCDC_SPRITE_SIZE_BIT) ? 16 : 8;
}
