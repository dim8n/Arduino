#include <dht.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <DS1302RTC.h>

#define DHT11_PIN 5
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
dht DHT;

// Set pins:  CE, IO,CLK
DS1302RTC RTC(8, 7, 6);
int strelka_i;

void display_strelka(int sec) {
  if(sec>=30) strelka_i=sec-30; else strelka_i = sec;
  
  if(strelka_i==0)  lcd.print("         <>         ");  // 1
  if(strelka_i==1)  lcd.print("        <<>>        ");  // 2
  if(strelka_i==2)  lcd.print("       <<<>>>       ");  // 3
  if(strelka_i==3)  lcd.print("      <<<<>>>>      ");  // 4
  if(strelka_i==4)  lcd.print("     <<<<<>>>>>     ");  // 5
  if(strelka_i==5)  lcd.print("    <<<<<<>>>>>>    ");  // 6
  if(strelka_i==6)  lcd.print("   <<<<<<<>>>>>>>   ");  // 7
  if(strelka_i==7)  lcd.print("  <<<<<<<<>>>>>>>>  ");  // 8
  if(strelka_i==8)  lcd.print("  <<<<<<<  >>>>>>>  ");  // 9
  if(strelka_i==9)  lcd.print("  <<<<<<    >>>>>>  ");  // 10
  if(strelka_i==10) lcd.print("  <<<<<      >>>>>  ");  // 11
  if(strelka_i==11) lcd.print("  <<<<        >>>>  ");  // 12
  if(strelka_i==12) lcd.print("  <<<          >>>  ");  // 13
  if(strelka_i==13) lcd.print("  <<            >>  ");  // 14
  if(strelka_i==14) lcd.print("  <              >  ");  // 15
  if(strelka_i==15) lcd.print("  >              <  ");  // 16
  if(strelka_i==16) lcd.print("  >>            <<  ");  // 17
  if(strelka_i==17) lcd.print("  >>>          <<<  ");  // 18
  if(strelka_i==18) lcd.print("  >>>>        <<<<  ");  // 19
  if(strelka_i==19) lcd.print("  >>>>>      <<<<<  ");  // 20
  if(strelka_i==20) lcd.print("  >>>>>>    <<<<<<  ");  // 21
  if(strelka_i==21) lcd.print("  >>>>>>>  <<<<<<<  ");  // 22
  if(strelka_i==22) lcd.print("  >>>>>>>><<<<<<<<  ");  // 23
  if(strelka_i==23) lcd.print("   >>>>>>><<<<<<<   ");  // 24
  if(strelka_i==24) lcd.print("    >>>>>><<<<<<    ");  // 25
  if(strelka_i==25) lcd.print("     >>>>><<<<<     ");  // 26
  if(strelka_i==26) lcd.print("      >>>><<<<      ");  // 27
  if(strelka_i==27) lcd.print("       >>><<<       ");  // 28
  if(strelka_i==28) lcd.print("        >><<        ");  // 29
  if(strelka_i==29) lcd.print("         ><         ");  // 30
 }
 
void setup() {
  Serial.begin(9600); // инициализация порта
  lcd.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight
  
  for(int i = 0; i< 3; i++) {lcd.backlight(); delay(250); lcd.noBacklight(); delay(250); } lcd.backlight();
}
 
void loop() {
  tmElements_t tm;
  RTC.read(tm);

  int chk = DHT.read11(DHT11_PIN);  switch (chk)
  {
    case DHTLIB_OK: break;
    case DHTLIB_ERROR_CHECKSUM: Serial.print("Checksum error,\t"); break;
    case DHTLIB_ERROR_TIMEOUT: Serial.print("Time out error,\t"); break;
    default: Serial.print("Unknown error,\t"); break;
  }
  // DISPLAY DATA
  Serial.print(tm.Day); Serial.print("/"); Serial.print(tm.Month); Serial.print("/"); Serial.print(tmYearToCalendar(tm.Year)); Serial.print(" ");
  Serial.print(tm.Hour); Serial.print(":"); Serial.print(tm.Minute); Serial.print(":"); Serial.print(tm.Second); Serial.print(" ");
  Serial.print("h:"); Serial.print(DHT.humidity, 1); Serial.print(" t:"); Serial.println(DHT.temperature, 1);

  lcd.setCursor(0,0);
  if(tm.Hour<10) {lcd.print("0"); lcd.print(tm.Hour);} else lcd.print(tm.Hour); lcd.print(":");
  if(tm.Minute<10) {lcd.print("0"); lcd.print(tm.Minute);} else lcd.print(tm.Minute); lcd.print(":");
  if(tm.Second<10) {lcd.print("0"); lcd.print(tm.Second);} else lcd.print(tm.Second);

  lcd.print("  ");
  if(tm.Day<10) {lcd.print("0"); lcd.print(tm.Day);} else lcd.print(tm.Day); lcd.print("/");
  if(tm.Month<10) {lcd.print("0"); lcd.print(tm.Month);} else lcd.print(tm.Month); lcd.print("/");
  lcd.print(tmYearToCalendar(tm.Year)); lcd.print("  ");
  lcd.setCursor(0,1);
  switch(tm.Wday){
    case 1: {lcd.print("      Monday    "); break;}
    case 2: {lcd.print("     Tuesday   "); break;}
    case 3: {lcd.print("     Wednesday "); break;}
    case 4: {lcd.print("     Thursday  "); break;}
    case 5: {lcd.print("      Friday    "); break;}
    case 6: {lcd.print("     Saturday  "); break;}
    case 7: {lcd.print("      Sunday    "); break;}
  }
  
  lcd.setCursor(0,2);
  display_strelka(tm.Second);
  
  lcd.setCursor(0,3); //Start at character 4 on line 0
  lcd.print("T: ");
  //lcd.setCursor(0,2); //Start at character 4 on line 0
  lcd.print(DHT.temperature);
  lcd.print("C");
  //lcd.setCursor(0,3); //Start at character 4 on line 0
  lcd.print(" H: ");
  //lcd.setCursor(0,3); //Start at character 4 on line 0
  lcd.print(DHT.humidity);
  lcd.print("%");
  
  delay(1000);
}
