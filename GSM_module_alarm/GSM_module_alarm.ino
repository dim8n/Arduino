#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
SoftwareSerial gsm(11, 12); // на контроллере RX, TX
int alarm_pin = 3;
int alarms = 0;

int adc_key_in  = 0;  // Кнопки дисплея
#define btnRIGHT  0   // константы кнопок
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons() {
 adc_key_in = analogRead(0);      // read the value from the sensor 
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 150)  return btnUP; 
 if (adc_key_in < 300)  return btnDOWN; 
 if (adc_key_in < 450)  return btnLEFT; 
 if (adc_key_in < 650)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}

void modemCheck(){
  lcd.clear(); lcd.setCursor(0,0); lcd.print("STATUS CHECK...");
  gsm.println("AT+CPAS");
  while(!gsm.find("+CPAS: 0")){ delay(500); gsm.println("AT+CPAS"); };
  lcd.clear(); lcd.setCursor(0,0); lcd.print("BOOT OK"); lcd.setCursor(0,1); lcd.print("CONNECTING...");
  
  gsm.println("AT+COPS?");
  while(!gsm.find("+COPS: 0,0,")){ delay(500); gsm.println("AT+COPS?"); }
  lcd.clear(); lcd.print("WORK");
}

void modemFastCheck(){
  gsm.println("AT+CPAS"); while(!gsm.find("+CPAS: 0")) { lcd.clear(); lcd.setCursor(0,0); lcd.print("STATUS CHECK..."); delay(500); gsm.println("AT+CPAS");}
  lcd.clear(); lcd.print("WORK");
}

void showOperator(){
  gsm.println("AT+COPS?"); //while(!gsm.find("+COPS: 0,0,")) {gsm.println("AT+COPS?"); delay(10);};
  if (gsm.available()) Serial.write(gsm.read());
}

void showStatus(){
  gsm.println("AT+CPAS"); //while(!gsm.find("+COPS: 0,0,")) {gsm.println("AT+COPS?"); delay(10);};
  if (gsm.available()) Serial.write(gsm.read());
}

void ring(){
  lcd.clear();
  lcd.print("RING!!! RING!!!");
}

void setup(){
  attachInterrupt(0, ring, FALLING);
  Serial.begin(9600);
  gsm.begin(9600);             /// скорость работы UART модема
  lcd.begin(16, 2);

  lcd.setCursor(0, 0);
  lcd.print("Hello!!!"); Serial.println("Hello!!!"); delay(3000);

  pinMode(alarm_pin, INPUT_PULLUP);     /// нога на датчик

  modemCheck();
}

void loop() {
  switch (read_LCD_buttons()) {
   case btnRIGHT: {
        showOperator();
        delay(250);
        break;}
   case btnLEFT: {
        showStatus();
        delay(250);
        break;}
   case btnUP: {
        delay(250);
        break;}
   case btnDOWN: {
        delay(250);
        break;}
   case btnSELECT: {
        delay(250);
        break;}
   case btnNONE: {
        if (digitalRead(alarm_pin)){
          alarms++;
          lcd.clear();
          lcd.setCursor(0,0); lcd.print("ALARM CALLING");
          gsm.println("ATD+375296425301;"); 
          delay(5000);
          lcd.clear(); lcd.print("END CALL"); delay(1000); lcd.clear(); lcd.print("WORK");
          lcd.setCursor(0,1); lcd.print(alarms); lcd.print(" alarms ");
          //gsm.println("AT+CPAS");
        }
        modemFastCheck(); delay(500);
        if (gsm.available()) Serial.write(gsm.read());
        if (Serial.available()) gsm.write(Serial.read());
        break;}
  }
}
