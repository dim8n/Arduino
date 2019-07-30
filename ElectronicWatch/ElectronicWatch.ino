#include <LiquidCrystal.h>
#include <EEPROM.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
#define LCD_Light 10 // Порт для управления подсетки дисплея
byte brightness = 127; // Значение яркости дисплея

int addr_br = 0;
int addr_h = 1;
int addr_m = 2;
int addr_al_h = 3;
int addr_al_m = 4;

unsigned long stored_h = 0;
unsigned long stored_m = 0;

unsigned long seconds = 0; // прошло всего секунд с момента включения
unsigned long time_in = 0; // установленное время
unsigned long ostatok = 0; // для расчета часов, минут, секунд
unsigned long chours = 0; // часы для вывода на экран
unsigned long cmins = 0; // минуты для вывода на экран
unsigned long csecs = 0; // секунды для вывода на экран
int al_hours = 0;
int al_minutes = 0;
unsigned long strelka_i = 0; // номер очередного эелемента для отображения на 2-й строке
unsigned long time_stamp4display = 0;
int edited_param = 0;

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

void display_time() {
  time_stamp4display = seconds;
  lcd.setCursor(0, 0);
  lcd.print("    ");
  ostatok = time_in; //unsigned long test1=ostatok;
  chours = ostatok/3600; ostatok=ostatok-(chours*3600); //unsigned long test2=ostatok;
  cmins = ostatok/60; ostatok=ostatok-(cmins*60); //unsigned long test3=ostatok;
  csecs = ostatok;
  if(chours<10) {lcd.print("0"); lcd.print(chours);} else lcd.print(chours); lcd.print(":");
  if(cmins<10) {lcd.print("0"); lcd.print(cmins);} else lcd.print(cmins); lcd.print(":");
  if(csecs<10) {lcd.print("0"); lcd.print(csecs);} else lcd.print(csecs);
  //lcd.print("    ");
  //lcd.print("-"); lcd.print(strelka_i); lcd.print(" ");
  
}

void display_strelka() {
  if(csecs>=30) strelka_i=csecs-30; else strelka_i = csecs;
  
  if(strelka_i==0)  lcd.print("       <>       ");  // 1
  if(strelka_i==1)  lcd.print("      <<>>      ");  // 2
  if(strelka_i==2)  lcd.print("     <<<>>>     ");  // 3
  if(strelka_i==3)  lcd.print("    <<<<>>>>    ");  // 4
  if(strelka_i==4)  lcd.print("   <<<<<>>>>>   ");  // 5
  if(strelka_i==5)  lcd.print("  <<<<<<>>>>>>  ");  // 6
  if(strelka_i==6)  lcd.print(" <<<<<<<>>>>>>> ");  // 7
  if(strelka_i==7)  lcd.print("<<<<<<<<>>>>>>>>");  // 8
  if(strelka_i==8)  lcd.print("<<<<<<<  >>>>>>>");  // 9
  if(strelka_i==9) lcd.print("<<<<<<    >>>>>>");  // 10
  if(strelka_i==10) lcd.print("<<<<<      >>>>>");  // 11
  if(strelka_i==11) lcd.print("<<<<        >>>>");  // 12
  if(strelka_i==12) lcd.print("<<<          >>>");  // 13
  if(strelka_i==13) lcd.print("<<            >>");  // 14
  if(strelka_i==14) lcd.print("<              >");  // 15
  if(strelka_i==15) lcd.print(">              <");  // 16
  if(strelka_i==16) lcd.print(">>            <<");  // 17
  if(strelka_i==17) lcd.print(">>>          <<<");  // 18
  if(strelka_i==18) lcd.print(">>>>        <<<<");  // 19
  if(strelka_i==19) lcd.print(">>>>>      <<<<<");  // 20
  if(strelka_i==20) lcd.print(">>>>>>    <<<<<<");  // 21
  if(strelka_i==21) lcd.print(">>>>>>>  <<<<<<<");  // 22
  if(strelka_i==22) lcd.print(">>>>>>>><<<<<<<<");  // 23
  if(strelka_i==23) lcd.print(" >>>>>>><<<<<<< ");  // 24
  if(strelka_i==24) lcd.print("  >>>>>><<<<<<  ");  // 25
  if(strelka_i==25) lcd.print("   >>>>><<<<<   ");  // 26
  if(strelka_i==26) lcd.print("    >>>><<<<    ");  // 27
  if(strelka_i==27) lcd.print("     >>><<<     ");  // 28
  if(strelka_i==28) lcd.print("      >><<      ");  // 29
  if(strelka_i==29) lcd.print("       ><       ");  // 30
  //if(strelka_i>30 | strelka_i<1) strelka_i=1;
  //lcd.print(EEPROM.read(addr_h));  lcd.print("  "); lcd.print(EEPROM.read(addr_m));  lcd.print("  ");
 }

void save_time() {
  EEPROM.write(addr_h, chours); EEPROM.write(addr_m, cmins);
}

void setup() {
  lcd.begin(16, 2);
  pinMode(LCD_Light, OUTPUT); // Подсветка дисплея
  for(int i = 0; i< 3; i++) {digitalWrite(LCD_Light, HIGH); delay(250); digitalWrite(LCD_Light, LOW); delay(250); } digitalWrite(LCD_Light, HIGH);;
  brightness = EEPROM.read(addr_br);
  analogWrite(LCD_Light, brightness);  // Восстановление сохраненной яркости экрана

  stored_h = EEPROM.read(addr_h); stored_m = EEPROM.read(addr_m); // восстановление сохраненного значения времени
  //al_hours = EEPROM.read(addr_al_h); al_minutes = EEPROM.read(addr_al_m);  // чтение значений будильника
  time_in = stored_h*3600+stored_m*60;
  if(time_in>=86400) time_in=0;
  display_time();
}

void loop() {
  seconds = millis()/1000;
  
  switch (read_LCD_buttons()) {
   case btnRIGHT: {
        //time_stamp=seconds; digitalWrite(LCD_Light, HIGH);
        if(edited_param>0) {edited_param+=1; if(edited_param>4) edited_param=1;}
        display_time();
        delay(250);
        break;}
   case btnLEFT: {
        //time_stamp=seconds; digitalWrite(LCD_Light, HIGH);
        if(edited_param>0) {edited_param-=1;  if(edited_param<1) edited_param=4;}
        display_time();
        delay(250);
        break;}
   case btnUP: {
        //time_stamp=seconds; digitalWrite(LCD_Light, HIGH);
        if(edited_param==1) {time_in+=3600;}
        if(edited_param==2) time_in+=60;
        if(edited_param==3) time_in-=ostatok;
        if(edited_param==4) {brightness+=20; analogWrite(LCD_Light, brightness); EEPROM.write(addr_br, brightness);}
        display_time();
        delay(250);
        break;}
   case btnDOWN: {
        //time_stamp=seconds; digitalWrite(LCD_Light, HIGH);
        if(edited_param==1) if(time_in<=3600) time_in+=82800; else time_in-=3600;
        if(edited_param==2) time_in-=60;
        if(edited_param==3) time_in-=ostatok;
        if(edited_param==4) {brightness-=20; analogWrite(LCD_Light, brightness); EEPROM.write(addr_br, brightness);}
        display_time();
        delay(250);
        break;}
   case btnSELECT: {
        //time_stamp=seconds; digitalWrite(LCD_Light, HIGH);
        if(edited_param==0) edited_param=1; else edited_param=0;
        display_time();
        delay(250);
        break;}
   case btnNONE: {break;}
  }
  if(time_in>=86400) time_in-=86400;
  //if((time_stamp+screen_light_delay)<=seconds) {digitalWrite(LCD_Light, LOW);}  // автоматическое отключение подсветки экрана

  if(seconds>time_stamp4display) {time_in+=1; if(time_in==86400) time_in=0; display_time(); if(csecs==0) save_time();}  // здесь идет прирашение времени!!!
  
  // Вывод второй строки
  lcd.setCursor(0, 1);
  switch (edited_param) {
    case 0: {
      display_strelka();
      break;
    }
    case 1: {
      lcd.print("   Set hours    ");
      break;
    }
    case 2: {
      lcd.print("  Set minutes   ");
      break;
    }
    case 3: {
      lcd.print(" Reset seconds  ");
      break;
    }
    case 4: {
      lcd.print("   Brightness   ");
      break;
    }
    case 5: {
      lcd.print(" Set alarm hours");
      break;
    }
    case 6: {
      lcd.print(" Set alarm m...s");
      break;
    }
  }
}

