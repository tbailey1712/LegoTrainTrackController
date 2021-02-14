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

#define DX1_ECHO 25
#define DX1_TRIGGER 26
#define DX2_ECHO    33
#define DX2_TRIGGER 32
#define DX3_ECHO    34
#define DX3_TRIGGER 14

#define DX_GAP_DELAY 500

#define LED_BUILTIN 2
#define SERVO_PIN   27

#define I2C_SDA 21
#define I2C_SCL 22
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define DX_DISTANCE 4
#define WL_ON_TIME 800

static const char* LOGTAG = "LTC-Devices";
String line1 = "", line2 = "", line3 = "", line4="", line5 = "";

// 0x3c for display
// 0x60 for Motor Feather

Adafruit_MotorShield motorShield(0x60); // Default address, no jumpers
Adafruit_StepperMotor *gateStepper1 = motorShield.getStepper(200, 1);

void moveGate1Forward() {  
  gateStepper1->onestep(FORWARD, DOUBLE);
}

void moveGate1Backward() {  
  gateStepper1->onestep(BACKWARD, DOUBLE);
}

AccelStepper gate1(moveGate1Forward, moveGate1Backward);

Devices::Devices() 
    : sensorDX1(DX1_TRIGGER, DX1_ECHO), 
      sensorDX2(DX2_TRIGGER, DX2_ECHO), 
      sensorDX3(DX3_TRIGGER, DX3_ECHO), 
      display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET)
{
}

void Devices::allLampsOff()
{
  mcp.digitalWrite(6, LOW);
  mcp.digitalWrite(7, LOW);
  setTS1(OFF);
  setTS2(OFF);
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
    dx1TrippedAt = -DX_GAP_DELAY;
  
    lampTest();
    clearLCD(2);
    clearLCD(3);
    clearLCD(4);

    enableWarningLights(false);
    ESP_LOGI(LOGTAG, "Train Controller System Ready");
}

void Devices::readDXSensors()
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
    printLCD(2, status);
}

boolean Devices::isTripped(uint8_t sensor)
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
void Devices::setTS1(uint8_t color)
{
    switch(color) {
      case RED:
        mcp.digitalWrite(0, HIGH);
        mcp.digitalWrite(1, LOW);
        mcp.digitalWrite(2, LOW);
        break;
      case YELLOW:
        mcp.digitalWrite(0, LOW);
        mcp.digitalWrite(1, HIGH);
        mcp.digitalWrite(2, LOW);
        break;
      case GREEN:
        mcp.digitalWrite(0, LOW);
        mcp.digitalWrite(1, LOW);
        mcp.digitalWrite(2, HIGH);
        break;
      case OFF:
        mcp.digitalWrite(0, LOW);
        mcp.digitalWrite(1, LOW);
        mcp.digitalWrite(2, LOW);
        break;
    }
}

void Devices::setTS2(uint8_t color)
{
    switch(color) {
      case RED:
        mcp.digitalWrite(3, HIGH);
        mcp.digitalWrite(4, LOW);
        mcp.digitalWrite(5, LOW);
        break;
      case YELLOW:
        mcp.digitalWrite(3, LOW);
        mcp.digitalWrite(4, HIGH);
        mcp.digitalWrite(5, LOW);
        break;
      case GREEN:
        mcp.digitalWrite(3, LOW);
        mcp.digitalWrite(4, LOW);
        mcp.digitalWrite(5, HIGH);
        break;
      case OFF:
        mcp.digitalWrite(3, LOW);
        mcp.digitalWrite(4, LOW);
        mcp.digitalWrite(5, LOW);
        break;

    }
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

  mcp.digitalWrite(6, HIGH);
  delay(lampDelay);
  mcp.digitalWrite(7, HIGH);
  delay(lampDelay);
  mcp.digitalWrite(6, LOW);
  mcp.digitalWrite(7, LOW);

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
    mcp.digitalWrite(6, LOW);
    mcp.digitalWrite(7, LOW);
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
      mcp.digitalWrite(6, LOW);
      mcp.digitalWrite(7, HIGH);
    }
    else 
    {
      mcp.digitalWrite(6, HIGH);
      mcp.digitalWrite(7, LOW);
    }
  }
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

