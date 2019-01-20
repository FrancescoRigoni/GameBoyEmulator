
#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <cstdint>
#include <SDL2/SDL.h>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "Devices/Joypad.hpp"

using namespace std;

class Screen {
private:
    Joypad *joypad;

    SDL_Window* window;
    SDL_Renderer* renderer;
    int currentScanLine = 0;

    mutex linesMutex;
    condition_variable zeroLinesCondition;
    condition_variable tooManyLinesCondition;
    queue<uint8_t *> linesQueue;
    void drawNextLine();

public:
    Screen(Joypad *);
    void pushLine(uint8_t *);
    uint8_t *popLine();
    void run();
};

#endif