
#ifndef __SOUNDCHANNEL_WAVE_H__
#define __SOUNDCHANNEL_WAVE_H__

#include <cstdint>

#include "Cpu/Memory.hpp"
#include "Util/LogUtil.hpp"
#include "Util/ByteUtil.hpp"
#include "Devices/IoDevice.hpp"
#include "Devices/Audio/LengthCounter.hpp"
#include "Devices/Audio/FrequencyCounter.hpp"

#define NR_30_SOUND_ON_OFF                   0xFF1A
#define NR_31_SOUND_LENGTH                   0xFF1B
#define NR_32_SOUND_OUTPUT_LEVEL             0xFF1C
#define NR_33_SOUND_MODE_FREQ_LO             0xFF1D
#define NR_34_SOUND_MODE_FREQ_HI             0xFF1E

#define WAVE_RAM_START                       0xFF30
#define WAVE_RAM_END                         0xFF3F

using namespace std;

class SoundChannelWave : public IoDevice {
private:
    uint8_t waveRam[0x10];

    uint8_t soundRegOnOff;
    uint8_t soundRegLength;
    uint8_t soundRegOutputLevel;
    uint8_t soundRegFrequencyLow;
    uint8_t soundRegFrequencyHigh;

    bool channelEnabled;
    int position = 0;

    FrequencyCounter frequencyTimer;
    LengthCounter lengthCounter;

    void checkForTrigger();
    void updatePeriod();

public:
    virtual void write8(uint16_t, uint8_t);
    virtual uint8_t read8(uint16_t);

    bool isChannelEnabled();
    float sample();
};

#endif