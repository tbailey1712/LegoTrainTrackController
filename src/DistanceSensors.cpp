#include <Arduino.h>
#include "DistanceSensors.h"

#define DX1_ECHO 25
#define DX1_TRIGGER 26
#define DX2_ECHO    33
#define DX2_TRIGGER 32
#define DX3_ECHO    34
#define DX3_TRIGGER 14
#define DX_GAP_DELAY 500
#define DX_DISTANCE 4


DistanceSensors::DistanceSensors()
   : sensorDX1(DX1_TRIGGER, DX1_ECHO), 
     sensorDX2(DX2_TRIGGER, DX2_ECHO), 
     sensorDX3(DX3_TRIGGER, DX3_ECHO)
{
     // DX1
    pinMode(DX1_TRIGGER, OUTPUT);
    pinMode(DX1_ECHO, INPUT);
    dx1TrippedAt = -DX_GAP_DELAY;

    pinMode(DX2_TRIGGER, OUTPUT);
    pinMode(DX2_ECHO, INPUT);
    dx2TrippedAt = -DX_GAP_DELAY;

    pinMode(DX3_TRIGGER, OUTPUT);
    pinMode(DX3_ECHO, INPUT);
    dx3TrippedAt = -DX_GAP_DELAY;

}

String DistanceSensors::readDXSensors()
{
  dx1 = sensorDX1.read(CM);    
  if (dx1 <= DX_DISTANCE) 
  {
    dx1TripWait = true;
    if (dx1TrippedAt == 0) {
      dx1TrippedAt = millis();
    }
    if (millis() > dx1TrippedAt + DX_GAP_DELAY) {
      dx1Tripped = true;
    }
  }
  else 
  {
    dx1TripWait = false;
    dx1TrippedAt = 0;
    dx1Tripped = false;
  }
  
  dx2 = sensorDX2.read(CM);    
  if (dx2 <= DX_DISTANCE) 
  {
    dx2TripWait = true;
    if (dx2TrippedAt == 0) {
      dx2TrippedAt = millis();
    }
    if (millis() > dx2TrippedAt + DX_GAP_DELAY) {
      dx2Tripped = true;
    }
  }
  else 
  {
    dx2TripWait = false;
    dx2TrippedAt = 0;
    dx2Tripped = false;
  }

  dx3 = sensorDX3.read(CM);    
  if (dx3 <= DX_DISTANCE) 
  {
    dx3TripWait = true;
    if (dx3TrippedAt == 0) {
      dx3TrippedAt = millis();
    }
    if (millis() > dx3TrippedAt + DX_GAP_DELAY) {
      dx3Tripped = true;
    }
  }
  else 
  {
    dx3TripWait = false;
    dx3TrippedAt = 0;
    dx3Tripped = false;
  }

    String status = "DX1 ";
    if (dx1Tripped) {
      status += "ON DX2 ";
    } else {
      status += "   DX2 ";
    }
    if (dx2Tripped) {
      status += "ON DX3 ";
    } else {
      status += "   DX3 ";
    }
    if (dx3Tripped) {
      status += "ON";
    } else {
      status += "  ";
    }
   
   return status;
}

boolean DistanceSensors::isTripped(uint8_t sensor)
{
    switch(sensor) {
        case 1:
            return dx1Tripped;
        case 2:
            return dx2Tripped;
        case 3:
            return dx3Tripped;
        default:
            return false;
    }
}

