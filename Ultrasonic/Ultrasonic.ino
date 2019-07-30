#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 10, 4, 5, 6, 7);

int duration, distance = 0;
#define trigPin 2  // Для ultrasonic
#define echoPin 3  // Для ultrasonic
#define LED 13

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

void loop() {
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/2.91;
  
  lcd.setCursor(0, 0);
  lcd.print(distance); lcd.print(" mm    ");
  lcd.setCursor(0, 1);
  if(distance<100) {digitalWrite(LED, HIGH); lcd.print("    ALARM!!!    ");} else {lcd.print("    All good    "); digitalWrite(LED, LOW);}
  Serial.println(distance); delay(100);
}

