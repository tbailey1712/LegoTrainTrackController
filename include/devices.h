#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <Ultrasonic.h>
#include <Adafruit_MotorShield.h>

#ifndef devices_h
#define devices_h

static const unsigned char PROGMEM wifi1_icon16x16[] =
{
	0b00000000, 0b00000000, //                 
	0b00000111, 0b11100000, //      ######     
	0b00011111, 0b11111000, //    ##########   
	0b00111111, 0b11111100, //   ############  
	0b01110000, 0b00001110, //  ###        ### 
	0b01100111, 0b11100110, //  ##  ######  ## 
	0b00001111, 0b11110000, //     ########    
	0b00011000, 0b00011000, //    ##      ##   
	0b00000011, 0b11000000, //       ####      
	0b00000111, 0b11100000, //      ######     
	0b00000100, 0b00100000, //      #    #     
	0b00000001, 0b10000000, //        ##       
	0b00000001, 0b10000000, //        ##       
	0b00000000, 0b00000000, //                 
	0b00000000, 0b00000000, //                 
	0b00000000, 0b00000000, //                 
};

static const unsigned char PROGMEM rail_left_icon16x16[] =
{
	0b00000000, 0b00000000, //1                 
	0b00000001, 0b11100000, //2        # ###     
	0b00000010, 0b11111000, //3       #  #####   
	0b00000100, 0b11111100, //4      #   ######  
	0b00001000, 0b00001110, //5     #        ### 
	0b00010000, 0b11100110, //6    #     ###  ## 
	0b00100000, 0b11110000, //7   #      ####    
	0b01111111, 0b00011000, //8  #######    ##   
	0b00100000, 0b11000000, //9   #      ##      
	0b00010000, 0b11100000, //0    #     ###     
	0b00001000, 0b00100000, //1     #      #     
	0b00000100, 0b10000000, //2      #   #       
	0b00000010, 0b10000000, //3       #  #       
	0b00000001, 0b00000000, //4        #        
	0b00000000, 0b00000000, //5                 
	0b00000000, 0b00000000, //6                 
};

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
//        Adafruit_MotorShield AFMS;
//        Adafruit_StepperMotor *myMotor;

        boolean flashWarningLights = false;
        unsigned long wlStart = 0;
        unsigned long wlTime = 0;
        void startupLCD();
        boolean lowerGates = false;

        long dx1TrippedAt = 0;
        boolean dx1Tripped = false;
        int dx1=0, dx2=0, dx3=0;
};

#endif