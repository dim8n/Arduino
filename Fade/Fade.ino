/*
 Fade
 
 This example shows how to fade an LED on pin 9
 using the analogWrite() function.
 
 This example code is in the public domain.
 */

int led1 = 9;           // the pin that the LED is attached to
int led2 = 10;
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
int freq = 300;

// the setup routine runs once when you press reset:
void setup()  { 
  // declare pin 9 to be an output:
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
} 

// the loop routine runs over and over again forever:
void loop()  { 
  analogWrite(led2, 125);
  delay(1000);
  analogWrite(led2, 0);
}

