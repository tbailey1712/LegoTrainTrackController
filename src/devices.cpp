#include <Arduino.h>
#include "Adafruit_MCP23017.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Adafruit_MCP23017.h"
#include <AccelStepper.h>
#include <Adafruit_MotorShield.h>
#include "esp_log.h"

#include "devices.h"
//#include "iot_iconset_16x16.h"

#define LED_TS1_RED     0
#define LED_TS1_YELLOW  1
#define LED_TS1_GREEN   2

#define LED_TS2_RED     3
#define LED_TS2_YELLOW  4
#define LED_TS2_GREEN   5

#define LED_WL_LEFT 6
#define LED_WL_RIGHT 7

#define LED_BUILTIN 2
#define SERVO_PIN   27

#define I2C_SDA 21
#define I2C_SCL 22
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define WL_ON_TIME 800

static const char* LOGTAG = "LTC-Devices";

// 0x3c for display
// 0x60 for Motor Feather

Adafruit_MotorShield motorShield(0x60); // Default address, no jumpers
Adafruit_StepperMotor *gateStepper1 = motorShield.getStepper(200, 1);
AccelStepper gate1(moveGate1Forward, moveGate1Backward);

Devices::Devices() 
      : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET),
        dxSensors()
{
}

void Devices::setup()
{
    ESP_LOGI(LOGTAG, "Beginning Device Setup");
    pinMode(LED_BUILTIN, OUTPUT); 
    
    motorShield.begin();
    gate1.setMaxSpeed(500.0);
    gate1.setSpeed(500.0);
    gate1.setAcceleration(1000.0);
    gate1.run();
    ESP_LOGI(LOGTAG, "Motor Shield Initialized");

    mcp.begin();
    ESP_LOGI(LOGTAG, "MCP I/O Expander Initialized");

    // TS1
    mcp.pinMode(LED_TS1_RED, OUTPUT);
    mcp.pinMode(LED_TS1_YELLOW, OUTPUT);
    mcp.pinMode(LED_TS1_GREEN, OUTPUT);
    // TS2
    mcp.pinMode(LED_TS2_RED, OUTPUT);
    mcp.pinMode(LED_TS2_YELLOW, OUTPUT);
    mcp.pinMode(LED_TS2_GREEN, OUTPUT);
    // WL
    mcp.pinMode(LED_WL_LEFT, OUTPUT);
    mcp.pinMode(LED_WL_RIGHT, OUTPUT);    

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    display.display();
    display.clearDisplay();
    startupLCD();
  
    lampTest();
    clearLCD(2);
    clearLCD(3);
    clearLCD(4);

    enableWarningLights(false);
    ESP_LOGI(LOGTAG, "Train Controller System Ready");
}

void Devices::readDXSensors()
{
  String status = dxSensors.readDXSensors();
  printLCD(2, status);
}

boolean Devices::isTripped(uint8_t sensor)
{
  return dxSensors.isTripped(sensor);
}

void Devices::setTS1(uint8_t color)
{
    switch(color) {
      case RED:
        mcp.digitalWrite(LED_TS1_RED, HIGH);
        mcp.digitalWrite(LED_TS1_YELLOW, LOW);
        mcp.digitalWrite(LED_TS1_GREEN, LOW);
        break;
      case YELLOW:
        mcp.digitalWrite(LED_TS1_RED, LOW);
        mcp.digitalWrite(LED_TS1_YELLOW, HIGH);
        mcp.digitalWrite(LED_TS1_GREEN, LOW);
        break;
      case GREEN:
        mcp.digitalWrite(LED_TS1_RED, LOW);
        mcp.digitalWrite(LED_TS1_YELLOW, LOW);
        mcp.digitalWrite(LED_TS1_GREEN, HIGH);
        break;
      case OFF:
        mcp.digitalWrite(LED_TS1_RED, LOW);
        mcp.digitalWrite(LED_TS1_YELLOW, LOW);
        mcp.digitalWrite(LED_TS1_GREEN, LOW);
        break;
    }
}

void Devices::setTS2(uint8_t color)
{
    switch(color) {
      case RED:
        mcp.digitalWrite(LED_TS2_RED, HIGH);
        mcp.digitalWrite(LED_TS2_YELLOW, LOW);
        mcp.digitalWrite(LED_TS2_GREEN, LOW);
        break;
      case YELLOW:
        mcp.digitalWrite(LED_TS2_RED, LOW);
        mcp.digitalWrite(LED_TS2_YELLOW, HIGH);
        mcp.digitalWrite(LED_TS2_GREEN, LOW);
        break;
      case GREEN:
        mcp.digitalWrite(LED_TS2_RED, LOW);
        mcp.digitalWrite(LED_TS2_YELLOW, LOW);
        mcp.digitalWrite(LED_TS2_GREEN, HIGH);
        break;
      case OFF:
        mcp.digitalWrite(LED_TS2_RED, LOW);
        mcp.digitalWrite(LED_TS2_YELLOW, LOW);
        mcp.digitalWrite(LED_TS2_GREEN, LOW);
        break;

    }
}

void Devices::allLampsOff()
{
  setTS1(OFF);
  setTS2(OFF);
}

void Devices::lampTest() 
{
  long lampDelay = 500;

  allLampsOff();
  printLCD(2, "    Testing Lamps    ");
  printLCD(3, "ABCDEFGHIJKLMNOPQRSTU");
  printLCD(5, "123456789012345678901");
  setTS1(RED);
  delay(lampDelay);
  setTS1(YELLOW);
  delay(lampDelay);
  setTS1(GREEN);
  delay(lampDelay);
  setTS1(OFF);

  setTS2(RED);
  delay(lampDelay);
  setTS2(YELLOW);
  delay(lampDelay);
  setTS2(GREEN);
  delay(lampDelay);
  setTS2(OFF);

  mcp.digitalWrite(LED_WL_LEFT, HIGH);
  delay(lampDelay);
  mcp.digitalWrite(LED_WL_RIGHT, HIGH);
  delay(lampDelay);
  mcp.digitalWrite(LED_WL_LEFT, LOW);
  mcp.digitalWrite(LED_WL_RIGHT, LOW);

}
void Devices::enableWarningLights(boolean flag)
{
  if (flag && !flashWarningLights) {
    wlStart = millis();
    printLCD(4, "Warning Lights On"); 
    flashWarningLights = true;
  } 
  if (!flag && flashWarningLights) {
    clearLCD(4); 
    mcp.digitalWrite(LED_WL_LEFT, LOW);
    mcp.digitalWrite(LED_WL_RIGHT, LOW);
    flashWarningLights = false;
  }
}

void Devices::checkWarningLights()
{
  if (flashWarningLights) {
    wlTime = millis() - wlStart;
    if (wlTime > (WL_ON_TIME * 2) ) 
    {
      wlStart = millis();
    }
    else if (wlTime > WL_ON_TIME) 
    {
      mcp.digitalWrite(LED_WL_LEFT, LOW);
      mcp.digitalWrite(LED_WL_RIGHT, HIGH);
    }
    else 
    {
      mcp.digitalWrite(LED_WL_LEFT, HIGH);
      mcp.digitalWrite(LED_WL_RIGHT, LOW);
    }
  }
}

void moveGate1Forward() {  
  gateStepper1->onestep(FORWARD, DOUBLE);
}

void moveGate1Backward() {  
  gateStepper1->onestep(BACKWARD, DOUBLE);
}

boolean Devices::areGatesDown()
{
    return (gate1.currentPosition() == 50);
}

boolean Devices::areGatesUp()
{
    return (gate1.currentPosition() == 0);
}

void Devices::checkGates()
{ 
    gate1.run();
    long gate1pos = gate1.currentPosition();
    // gate1position is a private var

    // 0 is UP, 50 is down
    if (lowerGates) {        

        if (gate1pos < 50) {            
            printLCD(5, "Lowering Gates");
            gate1.moveTo(gate1pos+1);
        }
        if (gate1pos == 50) {
            clearLCD(5);
        }
    }
    else {
        if (gate1pos > 0) {
            printLCD(5, "Raising Gates");
            gate1.moveTo(gate1pos-1);            
        }
        if (gate1pos == 0) {
            clearLCD(5);
        }
    }
}

void Devices::gatesDown()
{
    //enableWarningLights(true);
    lowerGates = true;
}

void Devices::gatesUp()
{
    lowerGates = false;
}

void Devices::startupLCD() 
{
    
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.drawBitmap(0, 0, rail_left_icon16x16, 16, 16, 1);
  display.drawBitmap(17, 0, rail_right_icon16x16, 16, 16, 1);
  display.println("     McDuck Rail");
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
      if (msg != line2) 
      {
        clearLCD(2);
        display.setCursor(0,16);    
        display.setTextColor(WHITE);
        display.println(msg);
        display.display();
        line2 = msg;
      }
      break;
    case 3:
      if (msg != line3)
      {
        clearLCD(3);
        display.setCursor(0,28);    
        display.println(msg);
        display.display();
        line3 = msg;
      }
      break;
    case 4:
      if (msg != line4)
      {
        clearLCD(4);
        display.setCursor(0,40);    
        display.println(msg);
        display.display();
        line4 = msg;
      }
      break;
    case 5:
      if (msg != line5)
      {
        clearLCD(5);
        display.setCursor(0,52);    
        display.println(msg);
        display.display();
        line5 = msg;
      }
      break;
  }
}

