#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP32Servo.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "esp_log.h"

#include <devices.h>

/****************************
 *  McDuck Labs
 *  Train Controller
 *  version
 *  Copyright
 * 
 *  TODO
 *  - Device Disco + Fail 0x20
 *  - Wifi connect and Time sync  https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/
 *  - Switch sets gate direction
 *  - Switch forces gates down
 *  - ERROR if started and sensors blocked
 * */

/*******  GPIO Pin Definitions *******/

#define TRAIN_CLEAR       0
#define TRAIN_APPROACHING 1
#define TRAIN_AT_GATES    2
#define TRAIN_ENTERING    3
#define TRAIN_CROSSING    4
#define TRAIN_CLEARING    5


/*******  Variables *******/
unsigned long nextUpdate = 0;
int heartbeatDelay = 1000;
int trainState = TRAIN_CLEAR;
Devices devices;
 
boolean isConnectedMCP23017() {
  byte mcp23017 = 0x20;
  Wire.beginTransmission(mcp23017);
  byte error = Wire.endTransmission();
  if (error == 0) {
    //Serial.println("MCP23017 Connected");
    return true;
  } else {
    Serial.println("MCP23017 NOT Connected");
    return false;
  }
}

void scanI2C() {
  byte mcp23017 = 0x20;
//  byte lcd = 0x3C;

  for (byte address = 1; address < 127; address++ ) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      Serial.println(address,HEX);

      if (address == mcp23017 ) {
        Serial.println(">> Found MCP23017 at 20");
      }
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }  
}



void setup() {
  Serial.begin(115200);
  Serial.println("Starting Up");

  devices.setup();

  devices.printLCD(5, "Ready");  
}

void loop() {
  
  devices.checkWarningLights();
  devices.checkGates();
  devices.readDXSensors();

  //
  // No sensors tripped, watch for DX1 to put down gates
  // TS1 Yellow
  // TS2 Red
  //
  if (trainState == TRAIN_CLEAR)  
  {
    devices.printLCD(3, "Train Clear");
    devices.setTS1(YELLOW);  
    devices.setTS2(RED);

    if (devices.areGatesUp()) {
      devices.enableWarningLights(false);
    }

    if ( devices.isTripped(1) ) 
    {
      trainState = TRAIN_APPROACHING;
      devices.enableWarningLights(true);
      devices.gatesDown();
    } 
  }
  //
  // Sensor DX1 tripped, train moving to DX2
  //
  else if (trainState == TRAIN_APPROACHING)
  {
    // If the train waits at DX1 on Yellow, 
    devices.printLCD(3, "Train Approaching");
    if (devices.areGatesDown())
    {      
      devices.setTS1(GREEN);  
      devices.setTS2(GREEN);
    }
    
    if (devices.isTripped(2))
    {
      devices.setTS1(RED);  
      trainState = TRAIN_AT_GATES;
    }
  }
  //
  // Sensor DX2 tripped, train is about to cross the road
  // DX1 & DX2 for long trains/close sensors
  //
  else if (trainState == TRAIN_AT_GATES)
  {
    devices.printLCD(3, "Train At Gates");

      // DX1 false DX 2 true
      if ( !devices.isTripped(1) && devices.isTripped(2) )
      {
        trainState = TRAIN_ENTERING;
      }
  }
  //
  // Sensor DX1 clear, DX2 tripped
  //
  else if (trainState == TRAIN_ENTERING)
  {
    devices.printLCD(3, "Train Entering");

    if (devices.isTripped(3)) 
    {
      devices.setTS1(RED);
      devices.setTS2(RED);
      trainState = TRAIN_CROSSING;
    }
  }
  else if (trainState == TRAIN_CROSSING)
  {
    devices.printLCD(3, "Train Crossing");

    if (!devices.isTripped(2) && devices.isTripped(3))
    {

      devices.setTS2(YELLOW);
      trainState = TRAIN_CLEARING;
    }
  }
  else if (trainState == TRAIN_CLEARING) 
  {
    devices.printLCD(3, "Train Clearing");

    if ( !devices.isTripped(3) )
    {
      devices.setTS2(RED);
      devices.gatesUp();  
      trainState = TRAIN_CLEAR;     
    }
  }
  /**
   * H E A R T B E A T   D I A G N O S T I C 
   **/
  if ( millis() > nextUpdate ) {
    digitalWrite(LED_BUILTIN, HIGH);

//    if (!isConnectedMCP23017()) {
//      devices.printLCD(5, "MCP23017 ERROR");
//    }
    nextUpdate = millis() + heartbeatDelay;
    delay(10);
    digitalWrite(LED_BUILTIN, LOW);
  }
  
}