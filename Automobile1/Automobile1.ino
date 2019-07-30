#include <AFMotor.h>
#include <Servo.h> 

int duration, distance = 0;
#define trigPin 2  // Для ultrasonic
#define echoPin 13  // Для ultrasonic

// Определение моторов
// DC motor on M2
AF_DCMotor motor_povorot(1);
AF_DCMotor motor_drive(2);
// DC hobby servo
Servo servo_eye;
int speed_all = 150;  //Постоянная скорость движения
bool flag_razgon = true;

char incomingByte;  // входящие данные bluetooth

void setup() {
  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Let's go!");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  // turn on servo
  servo_eye.attach(9);
   
  // turn on motors
  motor_povorot.setSpeed(0);
  motor_povorot.run(RELEASE);
  motor_drive.setSpeed(0);
  motor_drive.run(RELEASE);
 
  delay(2000);  // Задержка перед стартом
}

void loop() {
    /*incomingByte = Serial.read(); // считываем байт
    Serial.print("You press: "); Serial.println(incomingByte);
    if(incomingByte=='3') {Serial.println("Move left"); motor_step.step(1, FORWARD, MICROSTEP);}
    if(incomingByte=='4') {Serial.println("Move right"); motor_step.step(1, BACKWARD, MICROSTEP);}*/
    
   digitalWrite(trigPin, HIGH);
   delayMicroseconds(1000);
   digitalWrite(trigPin, LOW);
   duration = pulseIn(echoPin, HIGH);
   distance = (duration/2)/2.91;
   Serial.println(distance);// delay(100);
   
   if(distance>=200){
     motor_drive.run(FORWARD);
   }
   if(distance<200){
     motor_drive.run(BACKWARD); delay(250); motor_drive.setSpeed(0); motor_povorot.setSpeed(0); delay(500);
     motor_drive.run(BACKWARD); motor_drive.setSpeed(speed_all); delay(300); motor_povorot.run(FORWARD); motor_povorot.setSpeed(speed_all); delay(500);
     motor_drive.setSpeed(0); motor_povorot.setSpeed(0); delay(500);
     motor_drive.run(FORWARD); motor_drive.setSpeed(speed_all);
   }
}
