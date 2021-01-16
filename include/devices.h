#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <Ultrasonic.h>
#include <Adafruit_MotorShield.h>

#ifndef devices_h
#define devices_h

class Devices {
    public:
        Devices();

        void readDXSensors();
        boolean isTripped(uint8_t sensor);

        void allLampsOff();
        void setTS1(uint8_t red, uint8_t yellow, uint8_t green);
        void setTS2(uint8_t red, uint8_t yellow, uint8_t green);
        void setup();
        void lampTest();
        void enableWarningLights(boolean flag);
        void checkWarningLights();
        void checkGates();
        void gatesDown();
        void gatesUp();
        boolean areGatesUp();
        boolean areGatesDown();

        void clearLCD(int line);
        void printLCD(int line, String msg);

    private:
        Adafruit_MCP23017 mcp;  
        Ultrasonic sensorDX1;
        Ultrasonic sensorDX2; 
        Ultrasonic sensorDX3;	
        // SH1106 is the other, Try with the library "U8g2"
        Adafruit_SSD1306 display;
        Adafruit_MotorShield AFMS;
        Adafruit_StepperMotor *myMotor;

        boolean flashWarningLights = false;
        unsigned long wlStart = 0;
        unsigned long wlTime = 0;
        void startupLCD();
        boolean lowerGates = false;
        int gate1Position = 0;

        int dx1=0, dx2=0, dx3=0;
};

#endif