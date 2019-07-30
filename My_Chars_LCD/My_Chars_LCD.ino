#include <LiquidCrystal.h>

/* Константа: высота символа в точках */
enum { SYMBOL_HEIGHT = 8 };

/* Описываем наши собственные символы в виде массивов битовых масок.
 * Каждый символ - это восемь масок по пять бит.
 */
byte char0[SYMBOL_HEIGHT] =
{
  B00001,
  B00011,
  B00111,
  B01111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte char1[SYMBOL_HEIGHT] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
};
byte char2[SYMBOL_HEIGHT] =
{
  B10000,
  B11000,
  B11100,
  B11110,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte char3[SYMBOL_HEIGHT] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B01111,
  B00111,
  B00011,
  B00001,
};
byte char4[SYMBOL_HEIGHT] =
{
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte char5[SYMBOL_HEIGHT] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B11110,
  B11100,
  B11000,
  B10000,
};
byte char6[SYMBOL_HEIGHT] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};
byte char7[SYMBOL_HEIGHT] =
{
  B11111,
  B11111,
  B11111,
  B11111,
  B00000,
  B00000,
  B11111,
  B11111,
};

/* Объявляем объект нашего дисплея */
LiquidCrystal lcd(12, 11, 10, 4, 5, 6, 7);


void setup()
{
  /* Регистрируем собственные символы с кодами 0, 1 и 2 */
  lcd.createChar(0, char0);
  lcd.createChar(1, char1);
  lcd.createChar(2, char2);
  lcd.createChar(3, char3);
  lcd.createChar(4, char4);
  lcd.createChar(5, char5);
  lcd.createChar(6, char6);
  lcd.createChar(7, char7);
  
  /* Начинаем работу с дисплееем */
  lcd.begin(16, 2);
  
  /*lcd.write("\1"); // выводим наш собственный символ с кодом
  lcd.write("\2");
  lcd.write("\3");
  lcd.setCursor(0, 1);
  lcd.write("\4");
  lcd.write("\5");
  lcd.write("\6");
  
  lcd.setCursor(5, 0);
  lcd.write("\1"); // выводим наш собственный символ с кодом
  lcd.write("\7");
  lcd.setCursor(6, 1);
  lcd.write("\7");
  
  lcd.setCursor(8, 0);
  lcd.write("\1"); // выводим наш собственный символ с кодом
  lcd.write("\2");
  lcd.write("\3");
  lcd.setCursor(8, 1);
  lcd.write("\1");
  lcd.write("\5");
  lcd.write("\5");  */
}


void loop()
{
  lcd.setCursor(0,1); lcd.print("char test"); lcd.setCursor(0,0);
  //delay(4000);  
  /*for(int i=0; i++; i<=16){
     lcd.setCursor(0,0);
     for(int j=0; j++; j<=15){
       lcd.write(char(i*8+j)); delay(2000);
     }
   }*/
 //for(int j=0; j<=15; j++){  
   lcd.setCursor(0,0);
   for(int i=0; i<=15; i++) lcd.write(i); delay(1000);
 //}
}
