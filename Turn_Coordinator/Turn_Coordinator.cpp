#include "Turn_Coordinator.h"
#include "allocateMem.h"
#include "commandmessenger.h"
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <tc_main_gauge.h>
#include <tc_plane.h>
#include <slip_ball.h>
#include <slip_center_line.h>
#include <logo.h>

#define DEG2RAD 0.0174533
#define PANEL_COLOR 0x7BEE

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

TFT_eSprite TCmainSpr = TFT_eSprite(&tft);    // Main Sprite for Turn Coordinator
TFT_eSprite TCPlaneSpr = TFT_eSprite(&tft);   // Sprite for Turn Coordinator Airplane
TFT_eSprite slipBallSpr = TFT_eSprite(&tft);  // Sprite for slip ball
TFT_eSprite slipCtrLnSpr = TFT_eSprite(&tft); // Sprite for slip center line


/* **********************************************************************************
    This is just the basic code to set up your custom device.
    Change/add your code as needed.
********************************************************************************** */

Turn_Coordinator::Turn_Coordinator(uint8_t Pin1, uint8_t Pin2)
{
    _pin1 = Pin1;
    _pin2 = Pin2;
}

void Turn_Coordinator::begin()
{

  Serial.begin(115200);
  tft.begin();
  tft.setRotation(screenRotation);
  tft.fillScreen(PANEL_COLOR);
  tft.setPivot(320, 160);
  tft.setSwapBytes(true);
  tft.pushImage(160, 80, 160, 160, logo);
  delay(3000);


  TCmainSpr.createSprite(320, 320);
  TCmainSpr.setSwapBytes(true);
  TCmainSpr.fillSprite(TFT_BLACK);
  TCmainSpr.pushImage(0, 0, 320, 320, tc_main_gauge);

  slipBallSpr.createSprite(slip_ball_width, slip_ball_height);
  slipBallSpr.setSwapBytes(false);
  slipBallSpr.fillSprite(TFT_BLACK);
  slipBallSpr.pushImage(0, 0, slip_ball_width, slip_ball_height, slip_ball);

  slipCtrLnSpr.createSprite(slip_center_line_width, slip_center_line_height);
  slipCtrLnSpr.setSwapBytes(false);
  slipCtrLnSpr.fillSprite(TFT_BLACK);
  slipCtrLnSpr.pushImage(0, 0, slip_center_line_width, slip_center_line_height, slip_center_line);

  TCPlaneSpr.createSprite(tc_plane_width, tc_plane_height);
  TCPlaneSpr.setSwapBytes(true);
  TCPlaneSpr.fillSprite(TFT_BLACK);
  TCPlaneSpr.pushImage(0, 0, tc_plane_width, tc_plane_height, tc_plane);
  TCPlaneSpr.setPivot(tc_plane_width / 2, 36);
  tft.fillScreen(TFT_BLACK);
  digitalWrite(TFT_BL, LOW);

}

void Turn_Coordinator::attach(uint16_t Pin3, char *init)
{
    _pin3 = Pin3;
}

void Turn_Coordinator::detach()
{
    if (!_initialised)
        return;
    _initialised = false;
}

void Turn_Coordinator::set(int16_t messageID, char *setPoint)
{
    /* **********************************************************************************
        Each messageID has it's own value
        check for the messageID and define what to do.
        Important Remark!
        MessageID == -2 will be send from the board when PowerSavingMode is set
            Message will be "0" for leaving and "1" for entering PowerSavingMode
        MessageID == -1 will be send from the connector when Connector stops running
        Put in your code to enter this mode (e.g. clear a display)

    ********************************************************************************** */
    int32_t  data = atoi(setPoint);
    uint16_t output;

    // do something according your messageID
    switch (messageID) {
    case -1:
        // // tbd., get's called when Mobiflight shuts down
        setPowerSaveMode(true);
        break;
    case -2:
        // // tbd., get's called when PowerSavingMode is entered
        setPowerSaveMode((bool) atoi(setPoint));
        break;
    case 0:
        // output = (uint16_t)data;
        // data   = output;
        setTurnAngle(atof(setPoint));
        break;
    case 1:
        /* code */
        setSlipAngle(atof(setPoint));
        break;
    case 2:
        /* code */
        setInstrumentBrightnessRatio(atof(setPoint));
        break;
    case 100:
        /* code */
        setScreenRotation(atoi(setPoint));
        break;
    default:
        break;
    }
}

void Turn_Coordinator::update()
{
    // Do something which is required regulary
//   if(!powerSaveFlag)
//   {

//     analogWrite(TFT_BL, instrumentBrightness);

//     if(prevScreenRotation != screenRotation)
//     {
//         tft.setRotation(screenRotation);
//         prevScreenRotation = screenRotation;
//     }
//     drawGauge();

//    }
//    else digitalWrite(TFT_BL, LOW);

    analogWrite(TFT_BL, instrumentBrightness);

    if(prevScreenRotation != screenRotation)
    {
        tft.setRotation(screenRotation);
        prevScreenRotation = screenRotation;
    }

    if (screenRotation == 1 || screenRotation == 3)
    {
        tft.setViewport(80, 0, 320, 320, true);
        drawGauge();
    }
    else if (screenRotation == 0 || screenRotation == 2)
    {
        tft.setViewport(0, 80, 320, 320, true);
        drawGauge();

    }


}

void Turn_Coordinator::drawGauge()
{

  TCmainSpr.setPivot(160, 160);
  TCmainSpr.pushImage(0, 0, 320, 320, tc_main_gauge);

  ballXPos = scaleValue(slipAngle, 8, -8, 81, 209);
  ballYPos = cos((scaleValue(slipAngle / 2, 8, -8, -116, -244) - (-180)) * DEG_TO_RAD) * 36 + 153; // Approximation based on trial and error
  slipBallSpr.pushToSprite(&TCmainSpr, ballXPos, ballYPos, TFT_BLACK);

  slipCtrLnSpr.pushToSprite(&TCmainSpr, 144, 190, TFT_BLACK);

  TCPlaneSpr.pushRotated(&TCmainSpr, turnAngle, TFT_BLACK);

  TCmainSpr.pushSprite(0, 0);
  TCmainSpr.fillSprite(TFT_BLACK);

}


void Turn_Coordinator::setTurnAngle(float angle)
{
  turnAngle = angle;
}

void Turn_Coordinator::setInstrumentBrightnessRatio(float ratio)
{
  instrumentBrightnessRatio = ratio;
  instrumentBrightness = round(scaleValue(instrumentBrightnessRatio, 0, 1, 0, 255));
}

void Turn_Coordinator::setSlipAngle(double value)
{
  slipAngle = value;
}

void Turn_Coordinator::setScreenRotation(int rotation)
{
  if (rotation >= 0 || rotation <= 3)
    screenRotation = rotation;
}

void Turn_Coordinator::setPowerSaveMode(bool enabled)
{
    if(enabled)
    {
        digitalWrite(TFT_BL, LOW);
        tft.fillScreen(TFT_BLACK);
        powerSaveFlag = true;
    }
    else
    {
        analogWrite(TFT_BL, instrumentBrightness);
        powerSaveFlag = false;
    }
}

float Turn_Coordinator::scaleValue(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


