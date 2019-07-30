#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int seconds = 0;
int chours = 0;
int cmins = 0;
int csecs = 0;
int ostatok = 0;
int strelka_i = 0;
int duration, distance = 0;

int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define trigPin 12
#define echoPin 13

int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 150)  return btnUP; 
 if (adc_key_in < 300)  return btnDOWN; 
 if (adc_key_in < 450)  return btnLEFT; 
 if (adc_key_in < 650)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  lcd.setCursor(0, 0);

  switch (read_LCD_buttons()) {
   case btnRIGHT: {lcd.print("R"); Serial.println("Pressed RIGHT button"); break;}
   case btnLEFT: {lcd.print("L"); Serial.println("Pressed LEFT button"); break;}
   case btnUP: {lcd.print("U"); Serial.println("Pressed UP button"); break;}
   case btnDOWN: {lcd.print("D"); Serial.println("Pressed DOWN button"); break;}
   case btnSELECT: {lcd.print("S"); Serial.println("Pressed SELECT button"); break;}
   case btnNONE: {lcd.print("N"); Serial.println("No pressed buttons"); break;}
  }
  lcd.print("  ");

  seconds = millis()/1000;
  ostatok = seconds;
  chours = ostatok/3600; ostatok-=chours*3600;
  cmins = ostatok/60; ostatok-=cmins*60;
  csecs = ostatok;
  if(chours<10) {lcd.print("0"); lcd.print(chours);} else lcd.print(chours); lcd.print(":");
  if(cmins<10) {lcd.print("0"); lcd.print(cmins);} else lcd.print(cmins); lcd.print(":");
  if(csecs<10) {lcd.print("0"); lcd.print(csecs);} else lcd.print(csecs);
  lcd.print(" ");
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2)/2.91;
  Serial.print(distance);
  Serial.println(" mm");
  lcd.print(distance); lcd.print("  ");

  lcd.setCursor(0, 1);
  strelka_i = 1+seconds-(seconds/32)*32; //lcd.print(strelka_i);
  if(strelka_i<=16) {for(int i=1; i<strelka_i; i++) lcd.print(" "); lcd.print(">"); for(int i=0; i<16-strelka_i; i++) lcd.print(" ");}
  if(strelka_i>16) {for(int i=1; i<33-strelka_i; i++) lcd.print(" "); lcd.print("<"); for(int i=1; i<34-strelka_i; i++) lcd.print(" ");}
}

