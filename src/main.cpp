#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <ESP32Servo.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <devices.h>

/****************************
 *  McDuck Labs
 *  Train Controller
 *  version
 *  Copyright
 * 
 *  TODO
 *  - Multi file
 *  - Logging
 *  - Device Disco + Fail 0x20
 *  - Wifi connect and Time sync
 * */

/*******  GPIO Pin Definitions *******/


/*******  Variables *******/
unsigned long nextUpdate = 0;
int heartbeatDelay = 1000;


Devices devices;


Servo gateServo;  // create servo object to control a servo
 
boolean deviceDiscovery()
{
  
  return true;
}

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
  
  devices.showWarningLights();
  
  /**
   * H E A R T B E A T   D I A G N O S T I C 
   **/
  if ( millis() > nextUpdate ) {
    digitalWrite(LED_BUILTIN, HIGH);

    if (!isConnectedMCP23017()) {
      devices.printLCD(5, "MCP23017 ERROR");
    }

    if ( devices.isTripped(1) ) {
      devices.setTS1(LOW, LOW, HIGH);
      devices.setTS2(LOW, LOW, HIGH);
      devices.enableWarningLights(true);
    } else {
      devices.setTS1(HIGH, LOW, LOW);
      devices.setTS2(HIGH, LOW, LOW);
      devices.enableWarningLights(false);
    }
    nextUpdate = millis() + heartbeatDelay;
    digitalWrite(LED_BUILTIN, LOW);
  }
  
}