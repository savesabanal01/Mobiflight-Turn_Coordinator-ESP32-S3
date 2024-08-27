#pragma once

#include "Arduino.h"

class Turn_Coordinator
{
public:
    Turn_Coordinator(uint8_t Pin1, uint8_t Pin2);
    void begin();
    void attach(uint16_t Pin3, char *init);
    void detach();
    void set(int16_t messageID, char *setPoint);
    void update();

private:
    bool    _initialised;
    uint8_t _pin1, _pin2, _pin3;
    float turnAngle = 0;                     // angle of the turn angle from the simulator Initial Value
    float slipAngle = 0;                   // angle of the slip from the simulator
    float instrumentBrightnessRatio = 1;     // instrument brightness ratio from sim
    int instrumentBrightness = 255;            // instrument brightness based on ratio. Value between 0 - 255
    float prevInstrumentBrightnessRatio = 0; // previous value of instrument brightness. If no change do not set instrument brightness to avoid flickers
    float ballXPos = 0;                      // X position of the ball
    float ballYPos = 0;                      // YPosition of the ball
    float startTime = 0;
    float endTime = 0;
    float fps = 0;
    bool powerSaveFlag = false;
    int screenRotation = 3;
    int prevScreenRotation = 3;

    // Functions declrations
    float scaleValue(float x, float in_min, float in_max, float out_min, float out_max);
    void setTurnAngle(float angle);
    void setInstrumentBrightnessRatio(float ratio);
    void setSlipAngle(double value);
    void setScreenRotation(int value);
    void setPowerSaveMode(bool mode);
    void drawGauge();
};