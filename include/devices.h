#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <Ultrasonic.h>

#ifndef devices_h
#define devices_h

class Devices {
    public:
        Devices();

        boolean isTripped(uint8_t sensor);

        void allLampsOff();
        void setTS1(uint8_t red, uint8_t yellow, uint8_t green);
        void setTS2(uint8_t red, uint8_t yellow, uint8_t green);
        void setup();
        void lampTest();
        void enableWarningLights(boolean flag);
        void showWarningLights();

        void clearLCD(int line);
        void printLCD(int line, String msg);

    private:
        Adafruit_MCP23017 mcp;  
        Ultrasonic dx1;	
        // SH1106 is the other, Try with the library "U8g2"
        Adafruit_SSD1306 display;

        boolean flashWarningLights = false;
        unsigned long wlLeftOn = 0;
        unsigned long wlRightOn = 0;
        int wlOnTime = 800;
        void startupLCD();
};

#endif