/*
  Project: ESP32 Bluetooth Mobile Robot
  Board:
  – ESP32 Dev Module (Node32 Lite)
    https://my.cytron.io/p-node32-lite-wifi-and-bluetooth-development-kit
 */

#include <BluetoothSerial.h>
BluetoothSerial btSerial;
#define BT_NAME "ESP32BT-Idris" // Set bluetooth name

#define BUILTIN_LED 2 // LED is active low
#define M1A 33
#define M1B 25
#define M2A 26
#define M2B 27

#define PWM_FREQUENCY   1000
#define PWM_RESOLUTION  8
#define M1A_PWM_CHANNEL 0
#define M1B_PWM_CHANNEL 1
#define M2A_PWM_CHANNEL 2
#define M2B_PWM_CHANNEL 3

#define MAX_SPEED 200

boolean btConnected = false;
char key, previousKey;
int motorLeft, motorRight;
long previousMillis = 0;
int timeout = 1000;

void setup()
{
  pinMode(BUILTIN_LED, OUTPUT);
  
  ledcSetup(M1A_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M1B_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M2A_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(M2B_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  
  ledcAttachPin(M1A, M1A_PWM_CHANNEL);
  ledcAttachPin(M1B, M1B_PWM_CHANNEL);
  ledcAttachPin(M2A, M2A_PWM_CHANNEL);
  ledcAttachPin(M2B, M2B_PWM_CHANNEL);

  Serial.begin(115200);
  btSerial.begin(BT_NAME);
  
  Serial.println("ESP32 Bluetooth Mobile Robot");
  Serial.println();

  digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{
  if (btSerial.available()) {
    previousMillis = millis();
    
    char inChar = (char)btSerial.read();

    if (btConnected == false) {
      btConnected = true;
      digitalWrite(BUILTIN_LED, LOW); // Turn on led
      Serial.println("Bluetooth connected.");
    }

    if (inChar >= '0' && inChar <= '9') {
      key = inChar;
      if (key != previousKey) {
        
        switch (key) {
          case '0':
            Serial.println("Robot stop.");
            motorLeft = 0;
            motorRight = 0;
            break;

          case '1':
            Serial.println("Robot move forward.");
            motorLeft = 150;
            motorRight = 150;
            break;

          case '2':
            Serial.println("Robot move backward.");
            motorLeft = –150;
            motorRight = –150;
            break;

          case '3':
            Serial.println("Robot turn left.");
            motorLeft = –150;
            motorRight = 150;
            break;

          case '4':
            Serial.println("Robot turn right.");
            motorLeft = 150;
            motorRight = –150;
            break;
        }

        motorLeft = constrain(motorLeft, -MAX_SPEED, MAX_SPEED);
        motorRight = constrain(motorRight, -MAX_SPEED, MAX_SPEED);
        robotMove(motorLeft, motorRight);
        previousKey = key;
      }
    }
  }

  if (millis() – previousMillis > timeout &&
      btConnected == true) {

    Serial.println("Bluetooth disconnected.");
    robotMove(0, 0);
    digitalWrite(BUILTIN_LED, HIGH); // Turn off led
    btConnected = false;
  }
}

void robotMove(int speedLeft, int speedRight)
{
  speedLeft = constrain(speedLeft, –255, 255);
  speedRight = constrain(speedRight, –255, 255);
  
  if (speedLeft > 0) {
    int speedL = map(speedLeft, 0, 255, 255, 0);
    ledcWrite(M1A_PWM_CHANNEL, speedL);
    ledcWrite(M1B_PWM_CHANNEL, 255);
  }
  else {
    int speedL = map(speedLeft, –255, 0, 0, 255);
    ledcWrite(M1A_PWM_CHANNEL, 255);
    ledcWrite(M1B_PWM_CHANNEL, speedL);
  }

  if (speedRight > 0) {
    int speedR = map(speedRight, 0, 255, 255, 0);
    ledcWrite(M2A_PWM_CHANNEL, speedR);
    ledcWrite(M2B_PWM_CHANNEL, 255);
  }
  else {
    int speedR = map(speedRight, –255, 0, 0, 255);
    ledcWrite(M2A_PWM_CHANNEL, 255);
    ledcWrite(M2B_PWM_CHANNEL, speedR);
  }
}