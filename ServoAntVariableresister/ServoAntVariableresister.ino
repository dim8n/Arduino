#include <Servo.h>
#define SERVO1_PWM 10
Servo servo_1;

void setup()
{
  Serial.begin(9600);
  Serial.println("Simple Adafruit Motor Shield sketch");

  // Use the default "Servo" library of Arduino.
  // Attach the pin number to the servo library.
  // This might also set the servo in the middle position.
  servo_1.attach(SERVO1_PWM);
}


void loop()
{
  // Suppose there are two servo motors connected.
  // Let them move 180 degrees.
  int sensorValue = analogRead(A0);
  //Serial.print(sensorValue); // Serial.print(" - ");
  int deg = map(sensorValue, 0, 1023, 0, 179);
  Serial.println(deg);
  servo_1.write(deg);
  delay(100);
}

