// Adafruit Motor shield library
// copyright Adafruit Industries LLC, 2009
// this code is public domain, enjoy!

#include <AFMotor.h>

// Connect a stepper motor with 48 steps per revolution (7.5 degree)
// to motor port #2 (M3 and M4)
AF_Stepper motor(200, 2);
bool dir = false;
char incomingByte;  // входящие данные

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Stepper test!");
  motor.setSpeed(50);  //  rpm   
}

void loop() {
  //Serial.println("Single coil steps");
  
  //if (Serial.available() > 0) {  //если пришли данные
    incomingByte = Serial.read(); // считываем байт
    Serial.print("You press: "); Serial.println(incomingByte);
    if(incomingByte=='3') {Serial.println("Move left"); motor.step(1, FORWARD, MICROSTEP);}
    if(incomingByte=='4') {Serial.println("Move right"); motor.step(1, BACKWARD, MICROSTEP);}
    //delay(100);
  //}

  /*for(int i=1; i<200; i+=10){
    //dir=!dir;
    motor.step(i, FORWARD, MICROSTEP);
    delay(100);
    motor.step(i, BACKWARD, MICROSTEP);
    delay(200);  
  }
  for(int i=200; i>1; i-=10){
    //dir=!dir;
    motor.step(i, FORWARD, MICROSTEP);
    delay(100);
    motor.step(i, BACKWARD, MICROSTEP);
    delay(200);  
  }

  /*Serial.println("Double coil steps");
  motor.step(100, FORWARD, DOUBLE); 
  motor.step(100, BACKWARD, DOUBLE);

  Serial.println("Interleave coil steps");
  motor.step(100, FORWARD, INTERLEAVE); 
  motor.step(100, BACKWARD, INTERLEAVE); 

  Serial.println("Micrsostep steps");
  motor.step(100, FORWARD, MICROSTEP); 
  motor.step(100, BACKWARD, MICROSTEP); */
}
