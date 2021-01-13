#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_MCP23017.h"

#include "devices.h"

#define DX1_ECHO 25
#define DX1_TRIGGER 26
#define DX2_ECHO    33
#define DX2_TRIGGER 32
#define DX3_ECHO    34
#define DX3_TRIGGER 14
#define LED_BUILTIN 2
#define SERVO_PIN   27

#define I2C_SDA 21
#define I2C_SCL 22
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

// 0x3c for display

Devices::Devices() 
    : dx1(DX1_TRIGGER, DX1_ECHO), 
      display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void Devices::allLampsOff()
{
  mcp.digitalWrite(6, LOW);
  mcp.digitalWrite(7, LOW);
  setTS1(LOW,LOW,LOW);
  setTS2(LOW,LOW,LOW);
}

void Devices::setup()
{
    pinMode(LED_BUILTIN, OUTPUT); 

    mcp.begin();

    // TS1
    mcp.pinMode(0, OUTPUT);
    mcp.pinMode(1, OUTPUT);
    mcp.pinMode(2, OUTPUT);
    // TS2
    mcp.pinMode(3, OUTPUT);
    mcp.pinMode(4, OUTPUT);
    mcp.pinMode(5, OUTPUT);
    // WL
    mcp.pinMode(6, OUTPUT);
    mcp.pinMode(7, OUTPUT);    

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.display();
    display.clearDisplay();
    startupLCD();

     // DX1
    pinMode(DX1_TRIGGER, OUTPUT);
    pinMode(DX1_ECHO, INPUT);
  
    lampTest();
    clearLCD(2);
    clearLCD(3);
    clearLCD(4);
    enableWarningLights(false);
}

boolean Devices::isTripped(uint8_t sensor)
{
    boolean ret = false;
    switch(sensor) {
        case 1:
            ret = (dx1.read(CM) <= 10);
    }
    return ret;
}
void Devices::setTS1(uint8_t red, uint8_t yellow, uint8_t green)
{
    mcp.digitalWrite(0, red);
    mcp.digitalWrite(1, yellow);
    mcp.digitalWrite(2, green);
}

void Devices::setTS2(uint8_t red, uint8_t yellow, uint8_t green)
{
    mcp.digitalWrite(3, red);
    mcp.digitalWrite(4, yellow);
    mcp.digitalWrite(5, green);
}

void Devices::lampTest() 
{
  long lampDelay = 750;

  allLampsOff();
  printLCD(2, "    Testing Lamps    ");
  printLCD(3, "ABCDEFGHIJKLMNOPQRSTU");
  printLCD(5, "123456789012345678901");
  setTS1(HIGH, LOW, LOW);
  delay(lampDelay);
  setTS1(HIGH, HIGH, LOW);
  delay(lampDelay);
  setTS1(HIGH, HIGH, HIGH);
  delay(lampDelay);

  setTS2(HIGH, LOW, LOW);
  delay(lampDelay);
  setTS2(HIGH, HIGH, LOW);
  delay(lampDelay);
  setTS2(HIGH, HIGH, HIGH);
  delay(lampDelay);

  mcp.digitalWrite(6, HIGH);
  delay(lampDelay);
  mcp.digitalWrite(7, HIGH);
  delay(lampDelay);
  mcp.digitalWrite(6, LOW);
  mcp.digitalWrite(7, LOW);
  setTS1(LOW,LOW,LOW);

}
void Devices::enableWarningLights(boolean flag)
{
  if (flag && !flashWarningLights) {
    wlLeftOn = millis();
    wlRightOn = millis() + wlOnTime;
    //wlOnTime = 1000;   
    printLCD(4, "Warning Lights On"); 
    flashWarningLights = flag;
  } 
  if (!flag && flashWarningLights) {
    clearLCD(4); 
    mcp.digitalWrite(6, LOW);
    mcp.digitalWrite(7, LOW);
    flashWarningLights = flag;
  }
}

void Devices::showWarningLights()
{
  if (flashWarningLights) {
    if (millis() >= wlLeftOn) {
      mcp.digitalWrite(6, HIGH);
    }
    if (millis() >= (wlLeftOn + wlOnTime) ) {
      mcp.digitalWrite(6, LOW);
      wlLeftOn = millis() + wlOnTime;
    }
    if (millis() >= wlRightOn) {
      mcp.digitalWrite(7, HIGH);
    }
    if (millis() >= (wlRightOn + wlOnTime) ) {
      mcp.digitalWrite(7, LOW);
      wlRightOn = millis() + wlOnTime;
    }
  }
}

void Devices::startupLCD() 
{
    
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println("Lego Train Controller");
  display.drawLine(0, 12, display.width()-1, 12, WHITE);
  display.drawLine(0, 63, display.width()-1, 63, WHITE);

  printLCD(2, " from McDuck Labs");
  printLCD(4, "      v1.0");
  printLCD(5, "Starting Up");

    display.display();
}

void Devices::clearLCD(int line) 
{
  switch(line) {
    case 2:
      display.fillRect(0, 16, display.width()-1, 12, BLACK);
      display.display(); 
      break;
    case 3:
      display.fillRect(0, 28, display.width()-1, 12, BLACK);
      display.display(); 
      break;
    case 4:
      display.fillRect(0, 40, display.width()-1, 12, BLACK);
      display.display(); 
      break;
    case 5:
      display.fillRect(0, 52, display.width()-1, 10, BLACK);
      display.display(); 
      break;

  }
}

void Devices::printLCD(int line, String msg) 
{
  switch(line) {
    case 1:
      display.setCursor(0,0);    
      display.println(msg);
      break;
    case 2:
      clearLCD(2);
      display.setCursor(0,16);    
      display.setTextColor(WHITE);
      display.println(msg);
      display.display();
      break;
    case 3:
      clearLCD(3);
      display.setCursor(0,28);    
      display.println(msg);
      display.display();
      break;
    case 4:
      clearLCD(4);
      display.setCursor(0,40);    
      display.println(msg);
      display.display();
      break;
    case 5:
      clearLCD(5);
      display.setCursor(0,52);    
      display.println(msg);
      display.display();
      break;
  }
}

